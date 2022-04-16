/*
 * thsp_handler.cpp
 *
 *  Created on: 18.03.2019
 *      Author: jmajew
 */

#ifdef WIN32
	#include "thsp_handler.hpp"
#else
	#include "io/thsp_handler.hpp"
#endif


static uint8_t serialChecksumBuf( uint8_t checksum, uint8_t *pbeg, uint8_t *pend)
{
	for ( ; pbeg < pend; ++pbeg)
        checksum ^= *pbeg;

	return checksum;
}

bool ThspHandler::ProcessReceivedData( ThspMessage* pack, uint8_t c)
{
    switch ( this->GetState() )
    {
    	// waiting for '$' character
        default:
        case THSP_IDLE:
            if ( c == '$')
            {
                this->SetState( THSP_HEADER_START);
				mOffset = 0;
				mBuffer.checksum1 = 0;
	            mBuffer.sbuf.Init( mBuffer.data);
	            mBuffer.data[mOffset++] = c;
			}
            else
            {
                return false;
            }
            break;

        // waiting for 'M'
        case THSP_HEADER_START:
        	mBuffer.data[mOffset++] = c;
            switch (c)
            {
                case 'M':
                	this->SetState( THSP_HEADER_M);
                    this->SetVer( THSP_V1);
                    break;
                default:
                	this->SetState( THSP_IDLE);
                    break;
            }
            break;

        // waiting for '<' / '>'
        case THSP_HEADER_M:
        	this->SetState( THSP_HEADER_V1);
        	mBuffer.data[mOffset++] = c;
            switch (c)
            {
                case '<':
                	pack->packetType = THSP_PACKET_COMMAND;
                    break;
                case '>':
                	pack->packetType = THSP_PACKET_REPLY;
                    break;
				case '!':
					pack->packetType = THSP_PACKET_ERROR;
					break;
                default:
                	this->SetState( THSP_IDLE);
                    break;
            }
            break;

		// receive header
		case THSP_HEADER_V1:
			mBuffer.data[mOffset++] = c;
            mBuffer.checksum1 ^= c;
            if ( mOffset == THSP_MSG_DATA_OFFSET )
            {
				uint8_t size = mBuffer.data[3];
				uint8_t cmd  = mBuffer.data[4];

                // check incoming buffer size
                if ( size + THSP_MSG_DATA_OFFSET > THSP_PORT_INBUF_SIZE)
                {
                	this->SetState( THSP_IDLE);
                }
                else
                {
                    pack->dataSize = size;
                    pack->command = cmd;

                    //mspPort->offset = 0;                // re-use buffer
                    mBuffer.sbuf.SetPEnd( &(mBuffer.data[mOffset]) );

                    this->SetState( pack->dataSize > 0 ? THSP_PAYLOAD_V1 : THSP_CHECKSUM_V1 );
                }
            }
            break;

        case THSP_PAYLOAD_V1:
        	mBuffer.data[mOffset++] = c;
            mBuffer.checksum1 ^= c;
            mBuffer.sbuf.SetPEnd( &(mBuffer.data[mOffset]) );

            if ( mOffset == pack->dataSize + (uint16_t)THSP_MSG_DATA_OFFSET)
            {
            	this->SetState( THSP_CHECKSUM_V1);

//            	// FIXME :: reads from WRONG address
//            	mBuffer.sbuf.ReadData( pack->data, pack->dataSize );
            }
            break;

        case THSP_CHECKSUM_V1:
            if ( mBuffer.checksum1 == c)
            {
            	// TODO :: check if this is ok
            	memcpy( pack->data, mBuffer.data+THSP_MSG_DATA_OFFSET, pack->dataSize);
            	this->SetState( THSP_COMMAND_RECEIVED);
            }
            else
            {
            	// checksum failed
            	this->SetState( THSP_IDLE);
            }
            break;
    }

    return true;
}


bool ThspHandler::PackPackage( const ThspMessage* msg)
{
	// check data size for overflow
	if ( msg->dataSize >= (THSP_MAX_MESSAGE_SIZE  - THSP_MSG_DATA_OFFSET) ) // message longer than buffer
		return false;

	mBuffer.sbuf.Init(mBuffer.data);
	uint8_t* ptr = mBuffer.sbuf.GetPBeg();

	*ptr++ = '$';
	*ptr++ = 'M';
	*ptr++ = msg->packetType == THSP_PACKET_ERROR ? '!' : (msg->packetType == THSP_PACKET_REPLY ? '>' : '<');
	*ptr++ = (char)msg->dataSize;
	*ptr++ = msg->command;

	mBuffer.sbuf.SetPEnd(ptr);
	mBuffer.sbuf.WriteData( msg->data, msg->dataSize );
	mBuffer.checksum1 = *(mBuffer.sbuf.GetPEnd()) = serialChecksumBuf( 0, mBuffer.sbuf.GetPBeg() + 3, mBuffer.sbuf.GetPEnd());
	mBuffer.sbuf.SetPEnd( mBuffer.sbuf.GetPEnd() + 1 );

	return true;
}


