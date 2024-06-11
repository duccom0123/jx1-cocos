#ifndef KNpcAttribModifyH
#define KNpcAttribModifyH

class KNpc;

class KNpcAttribModify
{
public:
	KNpcAttribModify();
	~KNpcAttribModify();
	void	ModifyAttrib(KNpc* pNpc, void* pData,int nAttacker=0);
	void	MixPoisonDamage(KMagicAttrib* pDes, KMagicAttrib* pSrc);
private:
	void	(KNpcAttribModify::*ProcessFunc[magic_normal_end])(KNpc* pNpc, void* pData);	
	void	AllResMaxP(KNpc* pNpc, void* pData);
	void	PhysicsResMaxP(KNpc* pNpc, void* pData);
	void	ColdResMaxP(KNpc* pNpc, void* pData);
	void	FireResMaxP(KNpc* pNpc, void* pData);
	void	LightingResMaxP(KNpc* pNpc, void* pData);
	void	PoisonResMaxP(KNpc* pNpc, void* pData);
	void	LifePotionV(KNpc* pNpc, void* pData);
	void	ManaPotionV(KNpc* pNpc, void* pData);
	void	PoisonEnhanceP(KNpc* pNpc, void* pData);
	void	LightingEnhanceP(KNpc* pNpc, void* pData);
	void	FireEnhanceP(KNpc* pNpc, void* pData);
	void	ColdEnhanceP(KNpc* pNpc, void* pData);
	void	ArmorDefenseV(KNpc* pNpc, void* pData);
	void	LifeMaxV(KNpc* pNpc, void* pData);
	void	LifeMaxP(KNpc* pNpc, void* pData);  // 生命最大值
	void	LifeV(KNpc* pNpc, void* pData);
	void	LifeReplenishV(KNpc* pNpc, void* pData);
	void	ManaMaxV(KNpc* pNpc, void* pData);
	void	ManaMaxP(KNpc* pNpc, void* pData);
	void	ManaV(KNpc* pNpc, void* pData);
	void	ManaReplenishV(KNpc* pNpc, void* pData);
	void	StaminaMaxV(KNpc* pNpc, void* pData);
	void	StaminaMaxP(KNpc* pNpc, void* pData);
	void	StaminaV(KNpc* pNpc, void* pData);
	void	StaminaReplenishV(KNpc* pNpc, void* pData);
	void	StrengthV(KNpc* pNpc, void* pData);
	void	DexterityV(KNpc* pNpc, void* pData);
	void	VitalityV(KNpc* pNpc, void* pData);
	void	EnergyV(KNpc* pNpc, void* pData);
	void	LuckyV(KNpc* pNpc, void* pData);
	void	PoisonresP(KNpc* pNpc, void* pData);
	void	FireresP(KNpc* pNpc, void* pData);
	void	LightingresP(KNpc* pNpc, void* pData);
	void	PhysicsresP(KNpc* pNpc, void* pData);
	void	ColdresP(KNpc* pNpc, void* pData);
	void	FreezeTimeReduceP(KNpc* pNpc, void* pData);
	void	BurnTimeReduceP(KNpc* pNpc, void* pData);
	void	PoisonTimeReduceP(KNpc* pNpc, void* pData);
	void	PoisonDamageReduceV(KNpc* pNpc, void* pData);
	void	StunTimeReduceP(KNpc* pNpc, void* pData);
	void	FastWalkRunP(KNpc* pNpc, void* pData);
	void	VisionRadiusP(KNpc* pNpc, void* pData);
	void	FastHitRecoverV(KNpc* pNpc, void* pData);
	void	AllresP(KNpc* pNpc, void* pData);
	void	AttackRatingV(KNpc* pNpc, void* pData);
	void	AttackRatingP(KNpc* pNpc, void* pData);
	void	Ignoredefense_p(KNpc* pNpc, void* pData);
	void	AttackSpeedV(KNpc* pNpc, void* pData);
	void	CastSpeedV(KNpc* pNpc, void* pData);
	void	MeleeDamageReturnP(KNpc* pNpc, void* pData);
	void	MeleeDamageReturnV(KNpc* pNpc, void* pData);
	void	RangeDamageReturnV(KNpc* pNpc, void* pData);
	void	RangeDamageReturnP(KNpc* pNpc, void* pData);
	/*void	XinPhysicsDamageV(KNpc* pNpc, void* pData);
	void	XinFireDamageV(KNpc* pNpc, void* pData);
	void	XinColdDamageV(KNpc* pNpc, void* pData);
	void	XinLightingDamageV(KNpc* pNpc, void* pData);
	void	XinPoisonDamageV(KNpc* pNpc, void* pData);
	void	XinPhysicsDamageP(KNpc* pNpc, void* pData);*/
    void	AddPhysicsDamagePP(KNpc* pNpc, void* pData);
	void	SlowMissleB(KNpc* pNpc, void* pData);
	void	ChangeCampV(KNpc* pNpc, void* pData);
	void	PhysicsArmorV(KNpc* pNpc, void* pData);
	void	ColdArmorV(KNpc* pNpc, void* pData);
	void	FireArmorV(KNpc* pNpc, void* pData);
	void	PoisonArmorV(KNpc* pNpc, void* pData);
	void	LightingArmorV(KNpc* pNpc, void* pData);
	void	Damage2ManaP(KNpc* pNpc, void* pData);
	void	StealLifeP(KNpc* pNpc, void* pData);
	void	StealStaminaP(KNpc* pNpc, void* pData);
	void	StealManaP(KNpc* pNpc, void* pData);
	void	AllSkillV(KNpc* pNpc, void* pData);
	void	MetalSkillV(KNpc* pNpc, void* pData);
	void	WoodSkillV(KNpc* pNpc, void* pData);
	void	WaterSkillV(KNpc* pNpc, void* pData);
	void	FireSkillV(KNpc* pNpc, void* pData);
	void	EarthSkillV(KNpc* pNpc, void* pData);
	void	KnockBackP(KNpc* pNpc, void* pData);
	void	DeadlyStrikeP(KNpc* pNpc, void* pData);
	void	BadStatusTimeReduceV(KNpc* pNpc, void* pData);
	void	ManaShieldP(KNpc* pNpc, void* pData);
	void	FatallyStrikeP(KNpc* pNpc, void* pData);

	void	AddPhysicsMagic(KNpc* pNpc, void* pData);
	void	AddColdMagic(KNpc* pNpc, void* pData);
	void	AddFireMagic(KNpc* pNpc, void* pData);
	void	AddLightingMagic(KNpc* pNpc, void* pData);
	void	AddPoisonMagic(KNpc* pNpc, void* pData);
	void	fatallystrikeresP(KNpc* pNpc, void* pData);
	void	addskilldamage1(KNpc* pNpc, void* pData);  //技能的加长
	void	addskilldamage2(KNpc* pNpc, void* pData);
	void	addskilldamage3(KNpc* pNpc, void* pData);
	void	addskilldamage4(KNpc* pNpc, void* pData);
	void	addskilldamage5(KNpc* pNpc, void* pData);
	void	addskilldamage6(KNpc* pNpc, void* pData);
	void	dynamicmagicshieldV(KNpc* pNpc, void* pData);
	void	addstealfeatureskill(KNpc* pNpc, void* pData);
	void	lifereplenishP(KNpc* pNpc, void* pData);
	void	ignoreskillP(KNpc* pNpc, void* pData);
	void	poisondamagereturnV(KNpc* pNpc, void* pData);
	void	poisondamagereturnP(KNpc* pNpc, void* pData);
	void	returnskillP(KNpc* pNpc, void* pData);
	void	autoreplyskill(KNpc* pNpc, void* pData);
	void	skill_mintimepercastV(KNpc* pNpc, void* pData);
	void	mintimepercastonhorseV(KNpc* pNpc, void* pData);
	void	poison2decmanaP(KNpc* pNpc, void* pData);
	void	skill_appendskil(KNpc* pNpc, void* pData);
	void	hide(KNpc* pNpc, void* pData);
	void	clearnegativestate(KNpc* pNpc, void* pData);
	void	returnresP(KNpc* pNpc, void* pData);
	void	decPercasttimehorse(KNpc* pNpc, void* pData);
	void	decPercasttime(KNpc* pNpc, void* pData);
	void	enhance_autoSkill(KNpc* pNpc, void* pData);       //自动触发某技能的概率
	void	enhance_lifeP(KNpc* pNpc, void* pData);
	void	enhance_lifeV(KNpc* pNpc, void* pData);
	void	enhance_711_auto(KNpc* pNpc, void* pData);
	void	enhance_714_auto(KNpc* pNpc, void* pData);
	void	enhance_717_auto(KNpc* pNpc, void* pData);
	void	enhance_723_missP(KNpc* pNpc, void* pData);
	void	expenhanceP(KNpc* pNpc, void* pData);
	void	SerisesDamage(KNpc* pNpc, void* pData);
	void	autoskill(KNpc* pNpc, void* pData);
    void    block_rate(KNpc* pNpc, void* pData);                 // 档格
    void    enhancehit_rate(KNpc* pNpc, void* pData);            // 重击
	void    anti_block_rate(KNpc* pNpc, void* pData);            //=拆解格挡概率：#d1+%
	void    anti_enhancehit_rate(KNpc* pNpc, void* pData);       //=抵挡重击概率：#d1+%
	void    sorbdamage_p(KNpc* pNpc, void* pData);               //=抵消伤害:#d1+
	void    anti_poisonres_p(KNpc* pNpc, void* pData);           //=忽略对方毒防:#d1-%
	void    anti_fireres_p(KNpc* pNpc, void* pData);             //=忽略对方火防:#d1-%
	void    anti_lightingres_p(KNpc* pNpc, void* pData);         //=忽略对方雷防:#d1-%
	void    anti_physicsres_p(KNpc* pNpc, void* pData);          //=忽略对方普防:#d1-%
	void    anti_coldres_p(KNpc* pNpc, void* pData);             //=忽略对方冰防:#d1-%
	void    not_add_pkvalue_p(KNpc* pNpc, void* pData);          //=不增加PK值概率:#d1+%
	void    add_boss_damage(KNpc* pNpc, void* pData);            //=对黄金boss攻击伤害<color=orange>#d1+%<color>
	void    five_elements_enhance_v(KNpc* pNpc, void* pData);    //=五行强化值：#d1-点。强化对相克五行的克制效果
    void    five_elements_resist_v(KNpc* pNpc, void* pData); 
    void    skill_enhance(KNpc* pNpc, void* pData); //对攻击技能加成
    void    anti_allres_p(KNpc* pNpc, void* pData);
    void    add_alldamage_p(KNpc* pNpc, void* pData);
    void    auto_Revive_rate(KNpc* pNpc, void* pData);
	void    Add_neiphysicsenhance_p(KNpc* pNpc, void* pData);
	void    Addphysicsdamagevp(KNpc* pNpc, void* pData);
	void    Aaddfiredamagevp(KNpc* pNpc, void* pData);
	void    Addcolddamagevp(KNpc* pNpc, void* pData);
	void    Addlightingdamagevp(KNpc* pNpc, void* pData);
	void    Addpoisondamagevp(KNpc* pNpc, void* pData);
	void    Addphysicsmagicvb(KNpc* pNpc, void* pData);
	void    Addcoldmagicvp(KNpc* pNpc, void* pData);
	void    Addfiremagicv(KNpc* pNpc, void* pData);
	void    Addlightingmagicv(KNpc* pNpc, void* pData);
	void    Addpoisonmagicv(KNpc* pNpc, void* pData);
	void    Addphysicsmagicp(KNpc* pNpc, void* pData);
	void    addcreatnpc_v(KNpc* pNpc, void* pData);
	void    AddAttackRatingV(KNpc* pNpc, void* pData);
	void    AddAttackRatingP(KNpc* pNpc, void* pData);
	void    addzhuabu_v(KNpc* pNpc, void* pData);
	void    reduceskillcd1(KNpc* pNpc, void* pData);
	void    reduceskillcd2(KNpc* pNpc, void* pData);
	void    reduceskillcd3(KNpc* pNpc, void* pData);
	void    clearallcd(KNpc* pNpc, void* pData);
	void    addblockrate(KNpc* pNpc, void* pData);
	void    walkrunshadow(KNpc* pNpc, void* pData);
	void    returnskill2enemy(KNpc* pNpc, void* pData);
	void    manatoskill_enhance(KNpc* pNpc, void* pData);
	void    add_alldamage_v(KNpc* pNpc, void* pData);
	void    addskilldamage7(KNpc* pNpc, void* pData);
    void    ignoreattacrating_v(KNpc* pNpc, void* pData);
	void    alljihuo_v(KNpc* pNpc, void* pData);
	void    addexp_v(KNpc* pNpc, void* pData);
	void    doscript_v(KNpc* pNpc, void* pData);
	void    me2metaldamage_p(KNpc* pNpc, void* pData);
	void    metal2medamage_p(KNpc* pNpc, void* pData);
	void    me2wooddamage_p(KNpc* pNpc, void* pData);
	void    wood2medamage_p(KNpc* pNpc, void* pData);
	void    me2waterdamage_p(KNpc* pNpc, void* pData);
	void    water2medamage_p(KNpc* pNpc, void* pData);
	void    me2firedamage_p(KNpc* pNpc, void* pData);
	void    fire2medamage_p(KNpc* pNpc, void* pData);
	void    me2earthdamage_p(KNpc* pNpc, void* pData);
	void    earth2medamage_p(KNpc* pNpc, void* pData);
	void    ManaReplenishp(KNpc* pNpc, void* pData);
	void    fasthitrecover_p(KNpc* pNpc, void* pData);
	void    stuntrank_p(KNpc* pNpc, void* pData);	
	void    sorbdamage_v(KNpc* pNpc, void* pData);
	void    creatstatus_v(KNpc* pNpc, void* pData);	
	void    randmove(KNpc* pNpc, void* pData);
	void    addbaopoisondmax_p( KNpc* pNpc, void* pData);
    void    dupotion_v( KNpc* pNpc, void* pData);
	void    npcallattackSpeed_v( KNpc* pNpc, void* pData);
	void    eqaddskill_v( KNpc* pNpc, void* pData);
	void    autodeathskill( KNpc* pNpc, void* pData);
	void    autorescueskill( KNpc* pNpc, void* pData);
	void    staticmagicshield_p( KNpc* pNpc, void* pData);
	void    ignorenegativestate_p( KNpc* pNpc, void* pData);
	
	void    poisonres_yan_p( KNpc* pNpc, void* pData);
	void    fireres_yan_p( KNpc* pNpc, void* pData);
	void    lightingres_yan_p( KNpc* pNpc, void* pData);
	void    physicsres_yan_p( KNpc* pNpc, void* pData);
	void    coldres_yan_p( KNpc* pNpc, void* pData);
	void    lifemax_yan_v( KNpc* pNpc, void* pData);
	void    lifemax_yan_p( KNpc* pNpc, void* pData);
	void    manamax_yan_v( KNpc* pNpc, void* pData);
	void    manamax_yan_p( KNpc* pNpc, void* pData);

	void    sorbdamage_yan_p( KNpc* pNpc, void* pData);
	void    fastwalkrun_yan_p( KNpc* pNpc, void* pData);
	void    attackspeed_yan_v( KNpc* pNpc, void* pData);
	void    castspeed_yan_v( KNpc* pNpc, void* pData);
	void    allres_yan_p( KNpc* pNpc, void* pData);
	void    fasthitrecover_yan_v( KNpc* pNpc, void* pData);
	void    anti_physicsres_yan_p( KNpc* pNpc, void* pData);
	void    anti_poisonres_yan_p( KNpc* pNpc, void* pData);
    void    anti_coldres_yan_p( KNpc* pNpc, void* pData);
	void    anti_fireres_yan_p( KNpc* pNpc, void* pData);
	void    anti_lightingres_yan_p( KNpc* pNpc, void* pData);
	void    anti_allres_yan_p( KNpc* pNpc, void* pData);
	void    anti_sorbdamage_yan_p( KNpc* pNpc, void* pData);
	void    anti_hitrecover( KNpc* pNpc, void* pData);
	void    do_hurt_p( KNpc* pNpc, void* pData);
	void    addskillexp1( KNpc* pNpc, void* pData);
	void    anti_poisontimereduce_p( KNpc* pNpc, void* pData);
	void    anti_stuntimereduce_p( KNpc* pNpc, void* pData);
	void    addskilldamage8(KNpc* pNpc, void* pData);
	void    addskilldamage9(KNpc* pNpc, void* pData);
	void    addskilldamage10(KNpc* pNpc, void* pData);

	
	
	
};
extern KNpcAttribModify g_NpcAttribModify;
#endif
