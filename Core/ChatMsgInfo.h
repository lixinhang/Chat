#ifndef __CHATMSG_INFO_H_
#define __CHATMSG_INFO_H_


#define MAX_CHAT_NODE		3
#define KEEPLIVE_INTERVAL   30
#define MAX_WAIT_TIME		20
#define TICKER_TIME         1

#define ENTER               0
#define KEEPLIVE            0
#define SENDMSG				0
#define SENDMSG_PUB         0
#define SENDMSG_PRI         0

#define MSG_TCP_TIMEOUT		"访问超时"
#define MSG_RECONNECT_CHAT	"聊天重连"
#define MSG_USER_ENTER		"用户进入聊天室"
#define MSG_USER_EXIT		"用户离开聊天室"
#define MSG_LIVE_STOP		"结束直播"
#define MSG_LIVE_OK			"推流成功"
#define MSG_LIVE_FAILD		"推流失败"
#define MSG_LIVE_INTERRUPT	"推流中断"


#define ERR_UNKNOW_ERROR	"未知错误"
#define ERR_COOKIE_INVALID	"登录超时，请重新登录后再次进入畅秀阁。"
#define ERR_CONNECT_FAILD	"建立聊天室TCP连接失败"
#define ERR_ENTER_FAILD		"进入聊天室失败"
#define ERR_REENTRY_FAILD	"聊天室重连失败"
#define ERR_UPLOAD_FAILD	"直播视频数据上传失败，请尝试重新开播"


#define ERR_REPETITION_LOGIN	"账号重复进入直播间"

#endif