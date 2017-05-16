#include "ChatManager.hpp"


ChatManager * ChatManager::m_pInstance;

ChatManager::ChatManager()
{
    if(m_pChatRoom == NULL)
    {
        m_pChatRoom = CreateChatRoom(this);
    }
}

ChatManager::~ChatManager()
{
    if(m_pChatRoom != NULL)
    {
        DeleteChatRoom(m_pChatRoom);
        m_pInstance = NULL;
    }
}

void ChatManager::EnterChatRoom()
{
    if(!m_pChatRoom){ return; }
    m_pChatRoom->SetChatRoomInfo(m_RoomInfo);
    m_pChatRoom->SetToken(m_sToken);
    
    if(!m_pChatRoom->IsEntered())
    {
        m_pChatRoom->EnterChatRoom();
    }
}

void ChatManager::ExitChatRoom()
{
    if(m_pChatRoom->IsEntered())
    {
        m_pChatRoom->ExitChatRoom();
    }
}

void ChatManager::SendChatMsg(NSString *Msg, unsigned long long uid, bool bPrivate)
{
    if(m_pChatRoom->IsEntered())
    {
        m_pChatRoom->SendChatMsg([Msg UTF8String], uid, bPrivate);
    }
}

void ChatManager::setChatRoomInfo(CHATROOM_INFO info)
{
    m_RoomInfo.nPort = info.nPort;
    m_RoomInfo.strIp[0] = info.cIp;
    m_RoomInfo.nRoomId = info.rid;
    m_RoomInfo.nMasterId = info.uid;
    m_RoomInfo.nMasterNo = info.mid;
    
    m_sToken = info.cToken;
    
}

void ChatManager::OnChatRoom(ChatRoom::ChatRoomMsg crMsg, const char *strMsg)
{
    if(mController)
    {
        [mController doOnChatRoom:crMsg Msg:[NSString stringWithUTF8String:strMsg]];
    }
}

void ChatManager::setController(ChatControl* listener)
{
    this->mController = listener;
}

ChatManager *ChatManager::GetInstance()
{
    if(m_pInstance == NULL)  //判断是否第一次调用
        m_pInstance = new ChatManager();
    return m_pInstance;
}




