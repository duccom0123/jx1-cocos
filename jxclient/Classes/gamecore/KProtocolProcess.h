#ifndef KProtocolProcessH
#define	KProtocolProcessH
#include "cocos2d.h"
USING_NS_CC;
#include <time.h>
#include "KProtocol.h"
class KProtocolProcess
{
private:
	void (KProtocolProcess::*ProcessFunc[s2c_end])(BYTE* pMsg);
public:
	KProtocolProcess();
	~KProtocolProcess();
	void ProcessNetMsg(BYTE* pMsg);
private:
	void	s2cAccoutLoginResult(BYTE * pMsg);
	void	SyncCurNormalData(BYTE* pMsg);
	void	SyncWorld(BYTE* pMsg);
	void	SyncNpc(BYTE* pMsg);
	void	SyncNpcMin(BYTE* pMsg);
	void	SyncNpcMinPlayer(BYTE* pMsg);
	void	SyncPlayer(BYTE* pMsg);
	void	SyncPlayerMin(BYTE* pMsg);
	void	SyncCurPlayer(BYTE* pMsg);
	void	SyncObjectAdd(BYTE* pMsg);
	void	SyncObjectState(BYTE* pMsg);
	void	SyncObjectDir(BYTE* pMsg);
	void	SyncObjectRemove(BYTE* pMsg);
	void	SyncObjectTrap(BYTE* pMsg);
	void	NetCommandWalk(BYTE* pMsg);
	void	NetCommandRun(BYTE* pMsg);
	void	NetCommandJump(BYTE* pMsg);
	void	NetCommandChat(BYTE* pMsg);
	void	NetCommandSkill(BYTE* pMsg);
	void	NetCommandHurt(BYTE* pMsg);
	void	NetCommandDeath(BYTE* pMsg);
	void	NetCommandRemoveNpc(BYTE* pMsg);
	void	NetCommandChgCurCamp(BYTE* pMsg);
	void	NetCommandChgCamp(BYTE* pMsg);
	void	NetCommandSit(BYTE* pMsg);
	void	OpenSaleBox(BYTE* pMsg);
	void	OpenStoreBox(BYTE* pMsg);
	void	OpenResetPass(BYTE* pMsg);
	void	OpenStringUI(BYTE* pMsg); //stringbox
	void	GiftDT(BYTE* pMsg);
	void	s2cShowTeamInfo(BYTE* pMsg);
	void	s2cUpdataSelfTeamInfo(BYTE* pMsg);
	void	s2cApplyTeamInfoFalse(BYTE* pMsg);
	void	s2cCreateTeam(BYTE* pMsg);
	void	s2cApplyCreateTeamFalse(BYTE* pMsg);
	void	s2cSetTeamState(BYTE* pMsg);
	void	s2cApplyAddTeam(BYTE* pMsg);
	void	s2cTeamAddMember(BYTE* pMsg);
	void	s2cLeaveTeam(BYTE* pMsg);
	void	s2cTeamChangeCaptain(BYTE* pMsg);
	void	s2cSetFactionData(BYTE* pMsg);
	void	s2cLeaveFaction(BYTE* pMsg);
	void	s2cFactionSkillOpen(BYTE* pMsg);
	void	s2cGetChat(BYTE* pMsg);
	void	s2cGetLeadExp(BYTE* pMsg);
	void	s2cGetTeammateLevel(BYTE* pMsg);
	void	s2cLevelUp(BYTE* pMsg);
	void	s2cGetCurAttribute(BYTE* pMsg);
	void	s2cGetSkillLevel(BYTE* pMsg);
	void	s2cSyncAllSkill(BYTE * pMsg);
	void	s2cSyncMoney(BYTE* pMsg);
	void	s2cSyncXu(BYTE* pMsg);//xu
	void	s2cMoveItem(BYTE* pMsg);
	void	s2cRemoveItem(BYTE* pMsg);
	void	s2cSyncItem(BYTE* pMsg);
	void	SyncScriptAction(BYTE* pMsg);
	void	s2cChatGetApplyAddFriend(BYTE* pMsg);
	void	s2cSyncRoleList(BYTE* pMsg);
	void	s2cChatLoginFriendNoName(BYTE* pMsg);
	void	s2cChatLoginFriendName(BYTE* pMsg);
	void	s2cChatAddOneFriend(BYTE* pMsg);
	void	s2cChatBeRefusedAddFriend(BYTE* pMsg);
	void	s2cChatAddFriend(BYTE* pMsg);
	void	s2cChatAddFriendFail(BYTE* pMsg);
	void	s2cChatFriendOnline(BYTE* pMsg);
	void	s2cChatDeleteFriend(BYTE* pMsg);
	void	s2cChatFriendOffLine(BYTE* pMsg);
	void	SyncClientEnd(BYTE* pMsg);
	void	s2cTradeChangeState(BYTE* pMsg);
	void	s2cNpcSetMenuState(BYTE* pMsg);
	void	s2cTradeMoneySync(BYTE* pMsg);
	void	s2cTradeDecision(BYTE* pMsg);
	void	s2cPlayerExp(BYTE* pMsg);
	void	s2cChatScreenSingleError(BYTE* pMsg);
	void	s2cUpdateNpcState(BYTE * pMsg);
	void	s2cTeamInviteAdd(BYTE * pMsg);
	void	s2cTradePressOkSync(BYTE * pMsg);
	void	s2cPing(BYTE* pMsg);
	void	s2cDirectlyCastSkill(BYTE * pMsg);
	void	s2cShowMsg(BYTE * pMsg);
	void	SyncStateEffect(BYTE* pMsg);
	void	PlayerRevive(BYTE* pMsg);
	void	RequestNpcFail(BYTE* pMsg);
	void	s2cTradeApplyStart(BYTE* pMsg);
	void	s2cItemAutoMove(BYTE* pMsg);
	void	FinishedItemExchange(BYTE* pMsg);
	void	s2cChangeWeather(BYTE* pMsg);
	void	s2cPKSyncNormalFlag(BYTE* pMsg);
	void	s2cPKSyncEnmityState(BYTE* pMsg);
	void	s2cPKSyncExerciseState(BYTE* pMsg);
	void	s2cPKValueSync(BYTE* pMsg);
	void	s2cReputeValueSync(BYTE* pMsg);
	void	s2cFuYuanValueSync(BYTE* pMsg);
	void	s2cReBornValueSync(BYTE* pMsg);
	void	OpenMarketBox(BYTE* pMsg);
	void	NpcSleepSync(BYTE* pMsg);
	void	s2cViewEquip(BYTE* pMsg);
	void	LadderResult(BYTE* pMsg);
	void	LadderList(BYTE* pMsg);
	void	s2cTongCreate(BYTE* pMsg);
	void	ServerReplyClientPing(BYTE* pMsg);
	void	s2cNpcGoldChange(BYTE* pMsg);
	void	ItemChangeDurability(BYTE* pMsg);
	void	OpenTremble(BYTE* pMsg);
	void	NetCommandSetRankFF(BYTE* pMsg);
	void	s2cViewSellItem(BYTE* pMsg);
	void	s2cViewUpdateItem(BYTE* pMsg);
	void	s2cGetCouunt(BYTE* pMsg);
	void	s2cShopName(BYTE* pMsg);  // 摆摊广告
//	void	SyncInfo(BYTE* pMsg);
	void	SyncMaskLock(BYTE* pMsg);	// mat na
	void	SyncGive(BYTE* pMsg);
	void	SyncSecMov(BYTE* pMsg);
	void	SyncPKValue(BYTE* pMsg);
	void	SyncViewlianitem(BYTE* pMsg);  //链接
//	void	s2cLianUpdateItem(BYTE* pMsg); //链接更新
//	void	s2cGetLianCouunt(BYTE* pMsg);
	void	OpenWenGang(BYTE* pMsg); 
	void	Openstone(BYTE* pMsg); 
	void	s2cViewUpdateEquip(BYTE* pMsg); 
	void	s2cgettongback(BYTE* pMsg);
	void	s2csyncequipitem(BYTE* pMsg);
	void	s2ctaskcallback(BYTE* pMsg);
	void	s2copenjindutiao(BYTE* pMsg);  //打开进度条
	void	s2csyncitemposition(BYTE* pMsg); 
	void	s2copenduanzhao(BYTE* pMsg);
	void	s2csyncurupdata(BYTE* pMsg);
	void	s2cJinMaicallback(BYTE* pMsg);
	void	s2copenvipdazao(BYTE* pMsg);
	void	s2csynnpcinfocallback(BYTE* pMsg);
	void	s2cpautoMovecallback(BYTE* pMsg);
	void	s2cplyerguzicallback(BYTE* pMsg);
	void	s2csyncplayermap(BYTE* pMsg);
	void	s2csynconeclearstate(BYTE* pMsg);
	void	s2csyncnodataeffect(BYTE* pMsg);
	void	s2cnpcsetpos(BYTE* pMsg);
	void	s2cplayermissiondata(BYTE* pMsg);
	void	s2csyncrankdata(BYTE* pMsg);
	void	s2cSyncSuperShop(BYTE* pMsg);
	void	s2csyncitemshop(BYTE* pMsg);
	void	s2citemdatashop(BYTE* pMsg);
	void	s2ckickoutgame(BYTE* pMsg);
	void	s2cobjadd_vn(BYTE* pMsg);
	void	s2crolesyncvipvalue(BYTE* pMsg);

	void	s2cExtend(BYTE* pMsg);
	void	s2cExtendChat(BYTE* pMsg);
	void	s2cExtendFriend(BYTE* pMsg);
	void	s2cExtendTong(BYTE* pMsg);

};

extern KProtocolProcess g_ProtocolProcess;
#endif
