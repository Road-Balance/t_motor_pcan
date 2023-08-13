/* SPDX-License-Identifier: LGPL-2.1-only */
/*
 * 03_ManualReadWrite.cpp - PCANBasic Example: ManualReadWrite
 *
 * Copyright (C) 2001-2020  PEAK System-Technik GmbH <www.peak-system.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Contact:    <linux@peak-system.com>
 * Maintainer:  Fabrice Vergnaud <f.vergnaud@peak-system.com>
 * 	    	    Romain Tissier <r.tissier@peak-system.com>
 */
#include "pcan_pub_sub/ManualReadWrite.h"

#include <chrono>
#include <thread>

ManualReadWrite::ManualReadWrite()
{
	ShowConfigurationHelp(); // Shows information about this sample
	ShowCurrentConfiguration(); // Shows the current parameters configuration

	TPCANStatus stsResult;
	// Initialization of the selected channel
	if (IsFD)
		stsResult = CAN_InitializeFD(PcanHandle, BitrateFD);
	else
		stsResult = CAN_Initialize(PcanHandle, Bitrate);

	if (stsResult != PCAN_ERROR_OK)
	{
		std::cout << "Can not initialize. Please check the defines in the code.\n";
		ShowStatus(stsResult);
		std::cout << "\n";
		std::cout << "Closing...\n";
		std::cout << "Press any key to continue...\n";
		_getch();
		return;
	}

	// Reading messages...
	std::cout << "Successfully initialized.\n";
	// std::cout << "Start reading...\n";
	// std::cout << "Press any key to continue...\n";
	// _getch();
	// do
	// {
	// 	system("clear");
	// 	ReadMessages();
	// 	std::cout << "Do you want to read again? yes[y] or any other key to close\n";
	// } while (_getch() == 121);
}

ManualReadWrite::~ManualReadWrite()
{
	m_ThreadRun = false;
	if (m_ReadThread != NULL)
	{
		m_ReadThread->join();
		delete m_ReadThread;
	}
	CAN_Uninitialize(PCAN_NONEBUS);
}

void ManualReadWrite::WriteMessages(const int &user_input)
{
	TPCANStatus stsResult;

	if (IsFD)
		stsResult = WriteMessageFD();
	else
		stsResult = WriteMessage(user_input);

	// Checks if the message was sent
	if (stsResult != PCAN_ERROR_OK)
		ShowStatus(stsResult);
	else
		std::cout << "Message was successfully SENT\n";
}

TPCANStatus ManualReadWrite::WriteMessage(const int & user_input){
	TPCANMsg msgCanMessage;

	std::cout << "user_input[49-55] : " << user_input << std::endl;
	
	return CAN_Write(PcanHandle, &msgCanMessage);
}

TPCANStatus ManualReadWrite::WriteMessageFD()
{
	// Sends a CAN-FD message with standard ID, 64 data bytes, and bitrate switch
	TPCANMsgFD msgCanMessageFD;
	msgCanMessageFD.ID = 0x100;
	msgCanMessageFD.DLC = 15;
	msgCanMessageFD.MSGTYPE = PCAN_MESSAGE_FD | PCAN_MESSAGE_BRS;
	for (BYTE i = 0; i < 64; i++)
	{
		msgCanMessageFD.DATA[i] = i;
	}
	return CAN_WriteFD(PcanHandle, &msgCanMessageFD);
}

TPCANStatus ManualReadWrite::WriteROS2Message(unsigned char *data, int len, bool is_left)
{
	// TPCANStatus stsResult;
	TPCANMsg msgCanMessage;

	msgCanMessage.ID = is_left ? 0x60A : 0x60B;
	msgCanMessage.LEN = (BYTE)len;
	msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	msgCanMessage.DATA[0] = 0x23;
	msgCanMessage.DATA[1] = 0xFF;
	msgCanMessage.DATA[2] = 0x60;
	msgCanMessage.DATA[3] = 0x00;
	
	// printf("msgCanMessage.ID : %x\n", msgCanMessage.ID);

	for (int i = 4; i < 8; i++)
		msgCanMessage.DATA[i] = data[i-4];

	return CAN_Write(PcanHandle, &msgCanMessage);
}

TPCANStatus ManualReadWrite::Wakeup()
{
	// Sends a CAN message with extended ID, and 8 data bytes
	TPCANMsg msgCanMessage;

	// user_input == 49
	msgCanMessage.ID = 0x000;
	msgCanMessage.LEN = (BYTE)2;
	msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	msgCanMessage.DATA[0] = 0x81;
	msgCanMessage.DATA[1] = 0x0A;
	CAN_Write(PcanHandle, &msgCanMessage);

	msgCanMessage.ID = 0x000;
	msgCanMessage.LEN = (BYTE)2;
	msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	msgCanMessage.DATA[0] = 0x81;
	msgCanMessage.DATA[1] = 0x0B;
	CAN_Write(PcanHandle, &msgCanMessage);

	std::cout << "NMT 810A 810B" << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	// user_input == 50
	msgCanMessage.ID = 0x000;
	msgCanMessage.LEN = (BYTE)2;
	msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	msgCanMessage.DATA[0] = 0x01;
	msgCanMessage.DATA[1] = 0x0A;
	CAN_Write(PcanHandle, &msgCanMessage);

	msgCanMessage.ID = 0x000;
	msgCanMessage.LEN = (BYTE)2;
	msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	msgCanMessage.DATA[0] = 0x01;
	msgCanMessage.DATA[1] = 0x0B;
	CAN_Write(PcanHandle, &msgCanMessage);

	std::cout << "NMT 01A 01B" << std::endl;

	// msgCanMessage.ID = 0x70A;
	// msgCanMessage.LEN = (BYTE)1;
	// msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	// msgCanMessage.DATA[0] = 0x00;
	// CAN_Write(PcanHandle, &msgCanMessage);

	// std::cout << "HTBT 70A 00" << std::endl;

	// // TEMP
	// msgCanMessage.ID = 0x60A;
	// msgCanMessage.LEN = (BYTE)8;
	// msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	// msgCanMessage.DATA[0] = 0x2B;
	// msgCanMessage.DATA[1] = 0x0C;
	// msgCanMessage.DATA[2] = 0x10;
	// msgCanMessage.DATA[3] = 0x00;
	// msgCanMessage.DATA[4] = 0x64;
	// msgCanMessage.DATA[5] = 0x00;
	// msgCanMessage.DATA[6] = 0x00;
	// msgCanMessage.DATA[7] = 0x00;
	// CAN_Write(PcanHandle, &msgCanMessage);

	// std::cout << "18A 3180" << std::endl;

	// msgCanMessage.ID = 0x28A;
	// msgCanMessage.LEN = (BYTE)2;
	// msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	// msgCanMessage.DATA[0] = 0x00;
	// msgCanMessage.DATA[1] = 0x00;
	// CAN_Write(PcanHandle, &msgCanMessage);

	// std::cout << "28A 0000" << std::endl;

	// msgCanMessage.ID = 0x60A;
	// msgCanMessage.LEN = (BYTE)8;
	// msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	// msgCanMessage.DATA[0] = 0x2F;
	// msgCanMessage.DATA[1] = 0x60;
	// msgCanMessage.DATA[2] = 0x60;
	// msgCanMessage.DATA[3] = 0x00;
	// msgCanMessage.DATA[4] = 0x09;
	// msgCanMessage.DATA[5] = 0x00;
	// msgCanMessage.DATA[6] = 0x00;
	// msgCanMessage.DATA[7] = 0x00;
	// CAN_Write(PcanHandle, &msgCanMessage);

	// msgCanMessage.ID = 0x60B;
	// msgCanMessage.LEN = (BYTE)8;
	// msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	// msgCanMessage.DATA[0] = 0x2F;
	// msgCanMessage.DATA[1] = 0x60;
	// msgCanMessage.DATA[2] = 0x60;
	// msgCanMessage.DATA[3] = 0x00;
	// msgCanMessage.DATA[4] = 0x09;
	// msgCanMessage.DATA[5] = 0x00;
	// msgCanMessage.DATA[6] = 0x00;
	// msgCanMessage.DATA[7] = 0x00;
	// CAN_Write(PcanHandle, &msgCanMessage);

	// std::cout << "PDO 6060 09" << std::endl;

	// user_input == 51
	msgCanMessage.ID = 0x60A;
	msgCanMessage.LEN = (BYTE)8;
	msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	msgCanMessage.DATA[0] = 0x2B;
	msgCanMessage.DATA[1] = 0x40;
	msgCanMessage.DATA[2] = 0x60;
	msgCanMessage.DATA[3] = 0x00;
	msgCanMessage.DATA[4] = 0x06;
	msgCanMessage.DATA[5] = 0x00;
	msgCanMessage.DATA[6] = 0x00;
	msgCanMessage.DATA[7] = 0x00;
	CAN_Write(PcanHandle, &msgCanMessage);

	msgCanMessage.ID = 0x60B;
	msgCanMessage.LEN = (BYTE)8;
	msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	msgCanMessage.DATA[0] = 0x2B;
	msgCanMessage.DATA[1] = 0x40;
	msgCanMessage.DATA[2] = 0x60;
	msgCanMessage.DATA[3] = 0x00;
	msgCanMessage.DATA[4] = 0x06;
	msgCanMessage.DATA[5] = 0x00;
	msgCanMessage.DATA[6] = 0x00;
	msgCanMessage.DATA[7] = 0x00;
	CAN_Write(PcanHandle, &msgCanMessage);

	std::cout << "PDO 6040 06" << std::endl;

	// user_input == 52
	msgCanMessage.ID = 0x60A;
	msgCanMessage.LEN = (BYTE)8;
	msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	msgCanMessage.DATA[0] = 0x2B;
	msgCanMessage.DATA[1] = 0x40;
	msgCanMessage.DATA[2] = 0x60;
	msgCanMessage.DATA[3] = 0x00;
	msgCanMessage.DATA[4] = 0x07;
	msgCanMessage.DATA[5] = 0x00;
	msgCanMessage.DATA[6] = 0x00;
	msgCanMessage.DATA[7] = 0x00;
	CAN_Write(PcanHandle, &msgCanMessage);

	msgCanMessage.ID = 0x60B;
	msgCanMessage.LEN = (BYTE)8;
	msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	msgCanMessage.DATA[0] = 0x2B;
	msgCanMessage.DATA[1] = 0x40;
	msgCanMessage.DATA[2] = 0x60;
	msgCanMessage.DATA[3] = 0x00;
	msgCanMessage.DATA[4] = 0x07;
	msgCanMessage.DATA[5] = 0x00;
	msgCanMessage.DATA[6] = 0x00;
	msgCanMessage.DATA[7] = 0x00;
	CAN_Write(PcanHandle, &msgCanMessage);

	std::cout << "PDO 6040 07" << std::endl;

	// user_input == 53
	msgCanMessage.ID = 0x60A;
	msgCanMessage.LEN = (BYTE)8;
	msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	msgCanMessage.DATA[0] = 0x2B;
	msgCanMessage.DATA[1] = 0x40;
	msgCanMessage.DATA[2] = 0x60;
	msgCanMessage.DATA[3] = 0x00;
	msgCanMessage.DATA[4] = 0x0F;
	msgCanMessage.DATA[5] = 0x00;
	msgCanMessage.DATA[6] = 0x00;
	msgCanMessage.DATA[7] = 0x00;
	CAN_Write(PcanHandle, &msgCanMessage);

	msgCanMessage.ID = 0x60B;
	msgCanMessage.LEN = (BYTE)8;
	msgCanMessage.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	msgCanMessage.DATA[0] = 0x2B;
	msgCanMessage.DATA[1] = 0x40;
	msgCanMessage.DATA[2] = 0x60;
	msgCanMessage.DATA[3] = 0x00;
	msgCanMessage.DATA[4] = 0x0F;
	msgCanMessage.DATA[5] = 0x00;
	msgCanMessage.DATA[6] = 0x00;
	msgCanMessage.DATA[7] = 0x00;
	CAN_Write(PcanHandle, &msgCanMessage);

	std::cout << "PDO 6040 0F" << std::endl;

	return CAN_Write(PcanHandle, &msgCanMessage);
}

void ManualReadWrite::StartStatusFeedback(){
	m_ThreadRun = true;
	m_ReadThread = new std::thread(&ManualReadWrite::ThreadExecute, this);
}

TPCANMsg ManualReadWrite::ThreadExecute()
{
	int fd;
	TPCANMsg msgCanMessage;
	msgCanMessage.ID = 0x00;
	TPCANTimestamp CANTimeStamp;

	TPCANStatus stsResult = CAN_GetValue(PcanHandle, PCAN_RECEIVE_EVENT, &fd, sizeof(fd));

	if (stsResult != PCAN_ERROR_OK)
	{
		ShowStatus(stsResult);
		printf("========== PCAN_ERROR =========\n");
	}

	while (m_ThreadRun)
	{
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		// Checks for messages when an event is received
		// 여기서 pending 발생!!!
		int err = select(fd+1, &fds, NULL, NULL, NULL);
		if(err != -1 && FD_ISSET(fd, &fds)){
			TPCANStatus stsResult = CAN_Read(PcanHandle, &msgCanMessage, &CANTimeStamp);
			if ( msgCanMessage.ID == 0x48A ){
				// 여기서의 문제는 아니다.
				msgCanMessage_left_ = msgCanMessage;
				// printf("copy\n");
			}
			if ( msgCanMessage.ID == 0x48B ){
				msgCanMessage_right_ = msgCanMessage;
				// printf("copy\n");
			}
			// debug 
			// if (stsResult != PCAN_ERROR_QRCVEMPTY)
			// 	ProcessMessageCan(msgCanMessage_, CANTimeStamp);
		}
	}

	return msgCanMessage;
}

void ManualReadWrite::ReadMessages()
{
	TPCANStatus stsResult;

	// We read at least one time the queue looking for messages. If a message is found, we look again trying to
	// find more. If the queue is empty or an error occurr, we get out from the dowhile statement.
	do
	{
		stsResult = IsFD ? ReadMessageFD() : ReadMessage();
		if (stsResult != PCAN_ERROR_OK && stsResult != PCAN_ERROR_QRCVEMPTY)
		{
			ShowStatus(stsResult);
			return;
		}
	} while (!(stsResult & PCAN_ERROR_QRCVEMPTY));
}

TPCANStatus ManualReadWrite::ReadMessageFD()
{
	TPCANMsgFD CANMsg;
	TPCANTimestampFD CANTimeStamp;

	// We execute the "Read" function of the PCANBasic
	TPCANStatus stsResult = CAN_ReadFD(PcanHandle, &CANMsg, &CANTimeStamp);
	if (stsResult != PCAN_ERROR_QRCVEMPTY)
		// We process the received message
		ProcessMessageCanFD(CANMsg, CANTimeStamp);

	return stsResult;
}

TPCANStatus ManualReadWrite::ReadMessage()
{
	TPCANMsg CANMsg;
	TPCANTimestamp CANTimeStamp;

	// We execute the "Read" function of the PCANBasic
	TPCANStatus stsResult = CAN_Read(PcanHandle, &CANMsg, &CANTimeStamp);
	if (stsResult != PCAN_ERROR_QRCVEMPTY)
		// We process the received message
		ProcessMessageCan(CANMsg, CANTimeStamp);

	return stsResult;
}

void ManualReadWrite::ProcessMessageCan(TPCANMsg msg, TPCANTimestamp itsTimeStamp)
{
	UINT64 microsTimestamp = ((UINT64)itsTimeStamp.micros + 1000 * (UINT64)itsTimeStamp.millis + 0x100000000 * 1000 * itsTimeStamp.millis_overflow);

	std::cout << "Type: " << GetMsgTypeString(msg.MSGTYPE) << "\n";
	std::cout << "ID: " << GetIdString(msg.ID, msg.MSGTYPE) << "\n";
	char result[MAX_PATH] = { 0 };
	sprintf_s(result, sizeof(result), "%i", msg.LEN);
	std::cout << "Length: " << result << "\n";
	std::cout << "Time: " << GetTimeString(microsTimestamp) << "\n";
	std::cout << "Data: " << GetDataString(msg.DATA, msg.MSGTYPE, msg.LEN) << "\n";
	std::cout << "----------------------------------------------------------\n";
}

void ManualReadWrite::ProcessMessageCanFD(TPCANMsgFD msg, TPCANTimestampFD itsTimeStamp)
{
	std::cout << "Type: " << GetMsgTypeString(msg.MSGTYPE) << "\n";
	std::cout << "ID: " << GetIdString(msg.ID, msg.MSGTYPE) << "\n";
	std::cout << "Length: " << GetLengthFromDLC(msg.DLC) << "\n";
	std::cout << "Time: " << GetTimeString(itsTimeStamp) << "\n";
	std::cout << "Data: " << GetDataString(msg.DATA, msg.MSGTYPE, GetLengthFromDLC(msg.DLC)) << "\n";
	std::cout << "----------------------------------------------------------\n";
}

void ManualReadWrite::ShowConfigurationHelp()
{
	std::cout << "=========================================================================================\n";
	std::cout << "|                           PCAN-Basic ManualReadWrite Example                                |\n";
	std::cout << "=========================================================================================\n";
	std::cout << "Following parameters are to be adjusted before launching, according to the hardware used |\n";
	std::cout << "                                                                                         |\n";
	std::cout << "* PcanHandle: Numeric value that represents the handle of the PCAN-Basic channel to use. |\n";
	std::cout << "              See 'PCAN-Handle Definitions' within the documentation                     |\n";
	std::cout << "* IsFD: Boolean value that indicates the communication mode, CAN (false) or CAN-FD (true)|\n";
	std::cout << "* Bitrate: Numeric value that represents the BTR0/BR1 bitrate value to be used for CAN   |\n";
	std::cout << "           communication                                                                 |\n";
	std::cout << "* BitrateFD: String value that represents the nominal/data bitrate value to be used for  |\n";
	std::cout << "             CAN-FD communication                                                        |\n";
	std::cout << "=========================================================================================\n";
	std::cout << "\n";
}

void ManualReadWrite::ShowCurrentConfiguration()
{
	std::cout << "Parameter values used\n";
	std::cout << "----------------------\n";
	char buffer[MAX_PATH];
	FormatChannelName(PcanHandle, buffer, IsFD);
	std::cout << "* PCANHandle: " << buffer << "\n";
	if (IsFD)
		std::cout << "* IsFD: True\n";
	else
		std::cout << "* IsFD: False\n";
	ConvertBitrateToString(Bitrate, buffer);
	std::cout << "* Bitrate: " << buffer << "\n";
	std::cout << "* BitrateFD: " << BitrateFD << "\n";
	std::cout << "\n";
}

void ManualReadWrite::ShowStatus(TPCANStatus status)
{
	std::cout << "=========================================================================================\n";
	char buffer[MAX_PATH];
	GetFormattedError(status, buffer);
	std::cout << buffer << "\n";
	std::cout << "=========================================================================================\n";
}

void ManualReadWrite::FormatChannelName(TPCANHandle handle, LPSTR buffer, bool isFD)
{
	TPCANDevice devDevice;
	BYTE byChannel;

	// Gets the owner device and channel for a PCAN-Basic handle
	if (handle < 0x100)
	{
		devDevice = (TPCANDevice)(handle >> 4);
		byChannel = (BYTE)(handle & 0xF);
	}
	else
	{
		devDevice = (TPCANDevice)(handle >> 8);
		byChannel = (BYTE)(handle & 0xFF);
	}

	// Constructs the PCAN-Basic Channel name and return it
	char handleBuffer[MAX_PATH];
	GetTPCANHandleName(handle, handleBuffer);
	if (isFD)
		sprintf_s(buffer, MAX_PATH, "%s:FD %d (%Xh)", handleBuffer, byChannel, handle);
	else
		sprintf_s(buffer, MAX_PATH, "%s %d (%Xh)", handleBuffer, byChannel, handle);
}

void ManualReadWrite::GetTPCANHandleName(TPCANHandle handle, LPSTR buffer)
{
	strcpy_s(buffer, MAX_PATH, "PCAN_NONE");
	switch (handle)
	{
	case PCAN_PCIBUS1:
	case PCAN_PCIBUS2:
	case PCAN_PCIBUS3:
	case PCAN_PCIBUS4:
	case PCAN_PCIBUS5:
	case PCAN_PCIBUS6:
	case PCAN_PCIBUS7:
	case PCAN_PCIBUS8:
	case PCAN_PCIBUS9:
	case PCAN_PCIBUS10:
	case PCAN_PCIBUS11:
	case PCAN_PCIBUS12:
	case PCAN_PCIBUS13:
	case PCAN_PCIBUS14:
	case PCAN_PCIBUS15:
	case PCAN_PCIBUS16:
		strcpy_s(buffer, MAX_PATH, "PCAN_PCI");
		break;

	case PCAN_USBBUS1:
	case PCAN_USBBUS2:
	case PCAN_USBBUS3:
	case PCAN_USBBUS4:
	case PCAN_USBBUS5:
	case PCAN_USBBUS6:
	case PCAN_USBBUS7:
	case PCAN_USBBUS8:
	case PCAN_USBBUS9:
	case PCAN_USBBUS10:
	case PCAN_USBBUS11:
	case PCAN_USBBUS12:
	case PCAN_USBBUS13:
	case PCAN_USBBUS14:
	case PCAN_USBBUS15:
	case PCAN_USBBUS16:
		strcpy_s(buffer, MAX_PATH, "PCAN_USB");
		break;

	case PCAN_LANBUS1:
	case PCAN_LANBUS2:
	case PCAN_LANBUS3:
	case PCAN_LANBUS4:
	case PCAN_LANBUS5:
	case PCAN_LANBUS6:
	case PCAN_LANBUS7:
	case PCAN_LANBUS8:
	case PCAN_LANBUS9:
	case PCAN_LANBUS10:
	case PCAN_LANBUS11:
	case PCAN_LANBUS12:
	case PCAN_LANBUS13:
	case PCAN_LANBUS14:
	case PCAN_LANBUS15:
	case PCAN_LANBUS16:
		strcpy_s(buffer, MAX_PATH, "PCAN_LAN");
		break;

	default:
		strcpy_s(buffer, MAX_PATH, "UNKNOWN");
		break;
	}
}

void ManualReadWrite::GetFormattedError(TPCANStatus error, LPSTR buffer)
{
	// Gets the text using the GetErrorText API function. If the function success, the translated error is returned.
	// If it fails, a text describing the current error is returned.
	if (CAN_GetErrorText(error, 0x09, buffer) != PCAN_ERROR_OK)
		sprintf_s(buffer, MAX_PATH, "An error occurred. Error-code's text (%Xh) couldn't be retrieved", error);
}

void ManualReadWrite::ConvertBitrateToString(TPCANBaudrate bitrate, LPSTR buffer)
{
	switch (bitrate)
	{
	case PCAN_BAUD_1M:
		strcpy_s(buffer, MAX_PATH, "1 MBit/sec");
		break;
	case PCAN_BAUD_800K:
		strcpy_s(buffer, MAX_PATH, "800 kBit/sec");
		break;
	case PCAN_BAUD_500K:
		strcpy_s(buffer, MAX_PATH, "500 kBit/sec");
		break;
	case PCAN_BAUD_250K:
		strcpy_s(buffer, MAX_PATH, "250 kBit/sec");
		break;
	case PCAN_BAUD_125K:
		strcpy_s(buffer, MAX_PATH, "125 kBit/sec");
		break;
	case PCAN_BAUD_100K:
		strcpy_s(buffer, MAX_PATH, "100 kBit/sec");
		break;
	case PCAN_BAUD_95K:
		strcpy_s(buffer, MAX_PATH, "95,238 kBit/sec");
		break;
	case PCAN_BAUD_83K:
		strcpy_s(buffer, MAX_PATH, "83,333 kBit/sec");
		break;
	case PCAN_BAUD_50K:
		strcpy_s(buffer, MAX_PATH, "50 kBit/sec");
		break;
	case PCAN_BAUD_47K:
		strcpy_s(buffer, MAX_PATH, "47,619 kBit/sec");
		break;
	case PCAN_BAUD_33K:
		strcpy_s(buffer, MAX_PATH, "33,333 kBit/sec");
		break;
	case PCAN_BAUD_20K:
		strcpy_s(buffer, MAX_PATH, "20 kBit/sec");
		break;
	case PCAN_BAUD_10K:
		strcpy_s(buffer, MAX_PATH, "10 kBit/sec");
		break;
	case PCAN_BAUD_5K:
		strcpy_s(buffer, MAX_PATH, "5 kBit/sec");
		break;
	default:
		strcpy_s(buffer, MAX_PATH, "Unknown Bitrate");
		break;
	}
}

std::string ManualReadWrite::GetMsgTypeString(TPCANMessageType msgType)
{
	if ((msgType & PCAN_MESSAGE_STATUS) == PCAN_MESSAGE_STATUS)
		return "STATUS";

	if ((msgType & PCAN_MESSAGE_ERRFRAME) == PCAN_MESSAGE_ERRFRAME)
		return "ERROR";

	std::string strTemp;
	if ((msgType & PCAN_MESSAGE_EXTENDED) == PCAN_MESSAGE_EXTENDED)
		strTemp = "EXT";
	else
		strTemp = "STD";

	if ((msgType & PCAN_MESSAGE_RTR) == PCAN_MESSAGE_RTR)
		strTemp = (strTemp + "/RTR");
	else
		if (msgType > PCAN_MESSAGE_EXTENDED)
		{
			strTemp = (strTemp + " [ ");
			if (msgType & PCAN_MESSAGE_FD)
				strTemp = (strTemp + " FD");
			if (msgType & PCAN_MESSAGE_BRS)
				strTemp = (strTemp + " BRS");
			if (msgType & PCAN_MESSAGE_ESI)
				strTemp = (strTemp + " ESI");
			strTemp = (strTemp + " ]");
		}

	return strTemp;
}

std::string ManualReadWrite::GetIdString(unsigned int id, TPCANMessageType msgType)
{
	char result[MAX_PATH] = { 0 };
	// if ((msgType & PCAN_MESSAGE_EXTENDED) == PCAN_MESSAGE_EXTENDED)
	// {
	// 	sprintf_s(result, sizeof(result), "%08Xh", id);
	// 	return result;
	// }
	sprintf_s(result, sizeof(result), "%03Xh", id);
	return result;
}

int ManualReadWrite::GetLengthFromDLC(BYTE dlc)
{
	switch (dlc)
	{
	case 9: return 12;
	case 10: return 16;
	case 11: return 20;
	case 12: return 24;
	case 13: return 32;
	case 14: return 48;
	case 15: return 64;
	default: return dlc;
	}
}

std::string ManualReadWrite::GetTimeString(TPCANTimestampFD time)
{
	char result[MAX_PATH] = { 0 };
	double fTime = (time / 1000.0);
	sprintf_s(result, sizeof(result), "%.1f", fTime);
	return result;
}

std::string ManualReadWrite::GetDataString(BYTE data[], TPCANMessageType msgType, int dataLength)
{
	if ((msgType & PCAN_MESSAGE_RTR) == PCAN_MESSAGE_RTR)
		return "Remote Request";
	else
	{
		char strTemp[MAX_PATH] = { 0 };
		std::string result = "";
		for (int i = 0; i < dataLength; i++)
		{
			sprintf_s(strTemp, sizeof(strTemp), "%02X ", data[i]);
			result.append(strTemp);
		}

		return result;
	}
}