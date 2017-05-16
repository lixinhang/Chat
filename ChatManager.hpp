#ifndef ChatManager_hpp
#define ChatManager_hpp

#include "cxgChatRoom.h"
#include "CXGChat.h"

#import "MessageControl.h"

class ChatManager: ChatRoom::IChatRoomObserver
{
    
private:
    ChatManager();
    ~ChatManager();
    
    static ChatManager *m_pInstance;
    
public:
    
    static ChatManager *GetInstance();
    virtual void OnChatRoom(ChatRoom::ChatRoomMsg crMsg, const char* strMsg);
    
    void setController(ChatControl* listener);
    void setChatRoomInfo(CHATROOM_INFO Info);
    
    void EnterChatRoom();
    void SendChatMsg(NSString *Msg, unsigned long long uid, bool bPrivate);
    void ExitChatRoom();
    
private:
    
    std::string     m_sToken;
    ChatControl*    mController;
    ChatRoom::ChatRoomInfo  m_RoomInfo;
    ChatRoom::IChatRoom*    m_pChatRoom = NULL;
    
};

#endif /* ChatManager_hpp */
