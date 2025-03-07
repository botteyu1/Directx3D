#include "PreCompile.h"
#include "GameEngineNetClient.h"
#include "GameEngineDebug.h"

GameEngineNetClient::GameEngineNetClient() 
{
}

GameEngineNetClient::~GameEngineNetClient() 
{
    if (0 != ClientSocket)
    {
        closesocket(ClientSocket);
    }
}

bool GameEngineNetClient::Connect(const std::string& _IP, unsigned short _Port)
{
    // 내 프로그램이 네트워크를 사용할래요.
    WSAData WsaData;

    // 이 프로그램이 윈도우에게 서버를 사용하겠다고 알려주는 겁니다.
    int errorCode = WSAStartup(MAKEWORD(2, 2), &WsaData);
    if (SOCKET_ERROR == errorCode)
    {
        MsgBoxAssert("socket Error");
        return false;
    }

    ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (INVALID_SOCKET == ClientSocket)
    {
        return false;
    }

    SOCKADDR_IN ClientAdd;

    ClientAdd.sin_family = AF_INET;
    ClientAdd.sin_port = htons(30000);

    // 로컬호스트 ip 내컴퓨터에 내가 접속하겠다.
    std::string IP = _IP;

    if (SOCKET_ERROR == inet_pton(AF_INET, IP.c_str(), &ClientAdd.sin_addr))
    {
        return false;
    }

    int Len = sizeof(SOCKADDR_IN);

    // 접속 시간을 초과했습니다.
    if (SOCKET_ERROR == connect(ClientSocket, (const sockaddr*)&ClientAdd, Len))
    {
        MsgBoxAssert("커넥트에 실패했습니다.");
        return false;
    }

    if (0 == ClientSocket)
    {
        return false;
    }

    // 연결 됐고 서버가 보내는걸 받을 준비가 되었다.

    RecvThread.Start("Client RecvThread", std::bind_front(&GameEngineNet::RecvThreadFunction, ClientSocket, this));

    return true;
}

void GameEngineNetClient::RecvProcess(char* _Data)
{

}

void GameEngineNetClient::Disconnect(SOCKET _Socket)
{
    MessageBoxA(nullptr, "서버와의 연결이 끊어졌습니다.", "연결 종료", MB_OK);
}

void GameEngineNetClient::SendPacket(std::shared_ptr<GameEnginePacket> _Packet)
{
    GameEngineNet::SendPacket(ClientSocket, _Packet);
}