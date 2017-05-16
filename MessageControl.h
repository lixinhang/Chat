#import "CXGChat.h"

@interface ChatControl : NSObject

@property (nonatomic, weak) id<OnChatDelegate> delegate;
- (void) doOnMessage:(Message*) pkg;
- (void) doOnError:(int) errorCode Msg:(NSString*) msg;
- (void) doOnChatRoom:(int) msgCode Msg:(NSString*) msg;
@end
