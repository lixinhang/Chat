
typedef struct _CHATROOM_INFO
{
    short nPort;
    char* cIp;
    char* cToken;
    char* cname;
    
    unsigned long long uid;
    unsigned long long rid;
    unsigned long long mid;
}CHATROOM_INFO;

@interface User : NSObject
{
    NSInteger fensi;
    NSInteger caifu;
    NSInteger juese;
}

- (NSInteger) getFensi;
- (NSInteger) getCaifu;
- (NSInteger) getJuese;

- (void) setFensi:(NSInteger) fensi;
- (void) setCaifu:(NSInteger) caifu;
- (void) setJuese:(NSInteger) juese;

@end


@interface Gift : NSObject
{
    NSString* name;
    NSInteger count;
}

- (NSString*) getName;
- (NSInteger) getCount;
- (void) setName:(NSString*) name;
- (void) setCount:(NSInteger) count;

@end


@interface Message : NSObject
{
    NSString* mUid;
    NSString* mNickName;
    NSString* mMsg;
    NSInteger mType;
    
    Gift* mGift;
    User* mUser;
    
    //CHATROOM_INFO mRoomInfo;
}

- (id) init;

- (void) setUid:(NSString*) uid;
- (void) setNickName:(NSString*) name;
- (void) setMsg:(NSString*) msg;
- (void) setType:(NSInteger) type;

- (NSString*) getUid;
- (NSString*) getNickName;
- (NSString*) getMsg;
- (NSInteger) getType;
- (Gift*) getGift;
- (User*) getUser;

@end


@protocol OnChatDelegate <NSObject>
- (void)onMessage:(Message*) pkg;
- (void)onError:(long) errorCode Msg:(NSString*) msg;
- (void)onChatRoom:(int) msgCode Msg:(NSString*) msg;
@end

@interface ChatInterface : NSObject

- (int) EnterRoom;
- (int) LeaveRoom;

- (void) setReceiverObject:(id<OnChatDelegate>)delega;
- (void) SendMsgWithContent:(NSString*) Msg andToUser:(unsigned long long) uid andisPriavte:(bool) bPrivate;
- (void) setRoomInfoWith:(CHATROOM_INFO) info;

@end










