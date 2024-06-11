#ifndef KMagicAttribH
#define KMagicAttribH

#define	INVALID_ATTRIB	-1
// 枚举类型最后一个字母表示该数值的类型，v为整型，直接加的点数，p为百分比，b为布尔型
enum MAGIC_ATTRIB
{
//改变技能的属性
	magic_skill_begin,
	magic_skill_cost_v,								// 消耗MANA
	magic_skill_costtype_v,							//	
	magic_skill_mintimepercast_v,					// 每次发魔法的间隔时间
	magic_skill_misslenum_v,						// 子弹数量
	magic_skill_misslesform_v,		
	magic_skill_param1_v,		
	magic_skill_param2_v,
	magic_skill_attackradius,                        //magic_skill_reserve1, 攻击范围调整
	magic_skill_mintimepercastonhorse_v,             //magic_skill_reserve2,//骑马冷冻时间
	magic_skill_skillexp_v,                          //magic_skill_reserve3,技能熟练度
	magic_skill_desc,                                // 技能描述
	magic_skill_eventskilllevel,
	magic_skill_end,
	//改变发出子弹的属性		magic_skill_mintimepercastonhorse_v,
	magic_missle_begin,
	magic_missle_movekind_v,						// 子弹格式
	magic_missle_speed_v,							// 子弹飞行速度
	magic_missle_lifetime_v,						// 子弹的生命时间
	magic_missle_height_v,							// 子弹的高度
	magic_missle_damagerange_v,						// 子弹破坏范围
	magic_missle_radius_v,							// 子弹飞行半径
	magic_missle_missrate,                          // 子弹击中的概率
	magic_missle_hitcount,                          // 子弹击中的人数
	//magic_missle_reserve1,
	//magic_missle_reserve2,
	magic_missile_drag,//magic_missle_reserve3,
	magic_missle_reserve4,
	magic_missle_reserve5,
	magic_missle_end,
//装备属性	
	magic_item_begin,
	magic_weapondamagemin_v,
	magic_weapondamagemax_v,
	magic_armordefense_v,
	magic_durability_v,
	magic_requirestr,
	magic_requiredex,
	magic_requirevit,
	magic_requireeng,
	magic_requirelevel,
	magic_requireseries,
	magic_requiresex,	//男 或女 
	magic_requiremenpai,
	magic_weapondamageenhance_p,
	magic_armordefenseenhance_p,
	magic_requirementreduce_p,
	magic_indestructible_b,  //永不磨损
	magic_item_nouser,		 //不能使用的门派
	magic_item_needskill,	 //需要学会什么技能才能使用
	magic_item_needreborn,	 //需要转生次数
	magic_item_needtongban,	 //需要是同伴才可以装备
	magic_item_needbangzhu,	 //需要帮主
	magic_item_needcity,	 //需要城主
	magic_item_noseries,	 //某五行不能用的
	magic_item_reserve8,
	magic_item_reserve9,
	magic_item_reserve10,
	magic_item_end,
//主动攻击伤害属性---当前攻击技能--内外攻公用
	magic_damage_begin,		
	magic_attackrating_v,   //命中点
	magic_attackrating_p,   //命中率百分比
	magic_ignoredefense_p,  //忽略敌人闪避率
	magic_physicsdamage_v,  //攻击技能普点
	magic_colddamage_v,     //攻击技能冰点
	magic_firedamage_v,     //攻击技能火
	magic_lightingdamage_v, //攻击技能雷
	magic_poisondamage_v,   //攻击技能毒
	magic_magicdamage_v,    //
	magic_physicsenhance_p, //普点加强百分比
	magic_steallife_p,      //攻击技能
	magic_stealmana_p,
	magic_stealstamina_p,
	magic_knockback_p,          //增加攻击震退对手几率:#d1-%
	magic_deadlystrike_p,       //攻击致命一击率
	magic_fatallystrike_p,      //致命一击
	magic_stun_p,               //=眩晕时间加强
    //magic_coldenhance_p,        //增加迟缓时间百分比
	//magic_fireenhance_p,        //火时间加强百分比
	//magic_lightingenhance_p,    //雷晕时间加强加强
	//magic_poisonenhance_p,      //毒发间隔时间
	//magic_magicenhance_p,
	magic_xinphysicsdamage_v,   //=主动外功系普攻伤害:#d1+点
	magic_xinfiredamage_v,      //=主动外功系火攻伤害:#d1+点
	magic_xincolddamage_v,      //=主动外功系冰攻伤害:#d1+点
	magic_xinlightingdamage_v,  //=主动外功系雷攻伤害:#d1+点
	magic_xinpoisondamage_v,    //=主动外功系毒攻伤害:#d1+点/次
	magic_xinphysicsdamage_p,   //=主动外功系普攻伤害(%):#d1+%
    magic_xincoldmagic_v,       //=主动内功系冰攻伤害:#d1+点
//	magic_damage_reserve1,
//	magic_damage_reserve2,
//	magic_damage_reserve3,
//	magic_damage_reserve4,
//	magic_damage_reserve5,
//	magic_damage_reserve6,
//	magic_damage_reserve7,
	magic_addzhuabu_v,      //抓捕技能
//	magic_damage_reserve9,
	magic_autoattackskill,  //自动技能
	magic_seriesdamage_p,   //五行相克五行伤害
//	magic_magicenhance_p,   /////
	magic_damage_end,
//被动、状态、光环类属性--辅助状态-当前值
	magic_normal_begin,
	magic_lifemax_v,         //最大值点
	magic_lifemax_p,         //生命最大值百分比
	magic_life_v,            //当前生命
	magic_lifereplenish_v,   //生命恢复 点
	magic_manamax_v,
	magic_manamax_p,
	magic_mana_v,            //当前内力
	magic_manareplenish_v,   //内力恢复 点
	magic_staminamax_v,
	magic_staminamax_p,
	magic_stamina_v,
	magic_staminareplenish_v,
	magic_strength_v,
	magic_dexterity_v,
	magic_vitality_v,	        //增加外功增加多少点
	magic_energy_v,
	magic_poisonres_p,          //毒防 百分比
	magic_fireres_p,            //火方
	magic_lightingres_p,        //雷防
	magic_physicsres_p,         //普防
	magic_coldres_p,            //冰防  
	magic_freezetimereduce_p,   //减少冰冻时间百分比
	magic_burntimereduce_p,     //燃烧时间 火
	magic_poisontimereduce_p,   //中毒时间百分比
	magic_poisondamagereduce_v, //中毒伤害减少
	magic_stuntimereduce_p,     //减少眩晕时间百分比
	magic_fastwalkrun_p,        //移动速度
	magic_visionradius_p,
	magic_fasthitrecover_v,     //自己受伤动作时间减少
	magic_allres_p,             //全抗  点
	magic_attackspeed_v,        //外功攻速
	magic_castspeed_v,          //内功攻速
	magic_meleedamagereturn_v,
	magic_meleedamagereturn_p,  //减少近身伤害反弹百分比
	magic_rangedamagereturn_v,
	magic_rangedamagereturn_p,
	magic_addphysicsdamage_v,     // 状态外功普点
	magic_addfiredamage_v,        // 状态外火
	magic_addcolddamage_v,        // 状态外冰
	magic_addlightingdamage_v,    // 状态外雷
	magic_addpoisondamage_v,      // 状态外毒
	magic_addphysicsdamage_p,     // 状态外普百分比 --辅助技能
	magic_slowmissle_b,           // 减缓子弹飞行速度
	magic_changecamp_b,           // 迷惑敌人  改变阵营
	magic_physicsarmor_v,         // 物理护盾
	magic_coldarmor_v,
	magic_firearmor_v,
	magic_poisonarmor_v,
	magic_lightingarmor_v,
	magic_damagetomana_p,          //伤转内
	magic_lucky_v,		           //幸运
	magic_steallifeenhance_p,      //吸血
	magic_stealmanaenhance_p,      //吸内
	magic_stealstaminaenhance_p,   //吸体
	magic_allskill_v,              //加技能 等级
	magic_metalskill_v,            //加金系技能
	magic_woodskill_v,             //加木系技能
	magic_waterskill_v,            //加水系技能
	magic_fireskill_v,             //加火系技能
	magic_earthskill_v,            //加土系技能
	magic_knockbackenhance_p,      //命中敌人震退对方的几率:#d1-%
	magic_deadlystrikeenhance_p,   //攻击会心一击率
	magic_stunenhance_p,           //命中敌人致晕对方的几率:#d1+%
	magic_badstatustimereduce_v,   //不良状态时间增加
	magic_manashield_p,            //内力吸收伤害
	magic_adddefense_v,
	magic_adddefense_p,
	magic_fatallystrikeenhance_p,  //攻击致命一击率:#d1+%
	magic_lifepotion_v,		       //药品的喝血状态
	magic_manapotion_v,	           //药品的喝蓝状态 
	magic_physicsresmax_p,         //普防最大值                                                                                                                    
	magic_coldresmax_p,
	magic_fireresmax_p,
	magic_lightingresmax_p,
	magic_poisonresmax_p,
	magic_allresmax_p,		    //所有抗性最大值
	magic_coldenhance_p,        //增加对方迟缓时间百分比
	magic_fireenhance_p,        //火时间加强百分比
	magic_lightingenhance_p,    //雷晕时间加强加强
	magic_poisonenhance_p,      //毒发间隔时间
	magic_magicenhance_p,       
	magic_attackratingenhance_v, //命中率加强点
	magic_attackratingenhance_p, //命中率增加减少百分比
	magic_addphysicsmagic_v,	 //被动内功铺点
	magic_addcoldmagic_v,        //状态内冰
	magic_addfiremagic_v,        //状态内火
	magic_addlightingmagic_v,    //状态内雷
	magic_addpoisonmagic_v,      //状态内毒
	magic_fatallystrikeres_p,	 // 致命一击抵抗力
	magic_addskilldamage1,		 // 对「#l1-」技能加成#d3-%
	magic_addskilldamage2,       // 对「#l2-」技能加成#d3-%
	magic_expenhance_p,          // 杀死敌人获得经验 加 
	magic_addskilldamage3,
	magic_addskilldamage4,
	magic_addskilldamage5,
	magic_addskilldamage6,
	magic_dynamicmagicshield_v,		        // 化解伤害点数 
	magic_addstealfeatureskill,		        // 伪装技能剩余使用次数
	magic_lifereplenish_p,			        // 生命回复效率提升 百分比
	magic_ignoreskill_p,					// 无视降低抗性诅咒的概率
	magic_poisondamagereturn_v,				// 受到毒伤害时反弹点
	magic_poisondamagereturn_p,				// 受到毒伤害时反弹%
	magic_returnskill_p,					// 反弹所受到诅咒的概率
	magic_autoreplyskill,					// 击中时以<color=orange>#d9-%<color>的概率自动施放
//	magic_skill_mintimepercast_v,	        // 释放间隔时间
//	magic_autoattackskill,					// 自动释放技能
    magic_mintimepercastonhorse_v,	// 骑马释放间隔时间
	magic_poison2decmana_p,			// 使目标受到毒伤害的同时以 
	magic_skill_appendskill,		// 包含技能<color=water>#l1-<color>，最高等级<color=orange>#d3-级<color>
	magic_hide,						// 隐身<color=orange>#f2-秒<color>，释放任意武功后显形
	magic_clearnegativestate,       // 以<color=orange>#d1-%<color>的概率清除周围一定范围内友方玩家的异常状态
	magic_returnres_p,              // 对反弹伤害的抗性
	magic_dec_percasttimehorse,     // 减少#l3-冷却时间:马下<color=orange>#f7-秒<color>、马上<color=orange>#f4-秒<color>
	magic_dec_percasttime,          // 减少#l3-冷却时间:<color=orange>#f1-秒<color>
	magic_enhance_autoSkill,        // 触发「狮子吼」概率<color=orange>#d1+%<color>          //自动触发某技能的概率
	magic_enhance_life_p,           // 「倒虚天」生命回复效率提升<color=orange>#d1+%<color>  //对某技能的生命回复效率加成百分比
	magic_enhance_life_v,           // 「慈航普渡」生命恢复点数<color=orange>#d1+<color>点   //对某技能的生命回复效率加成点
	magic_enhance_711_auto,         // 触发「吸星魇」概率<color=orange>#d1+%<color>
	magic_enhance_714_auto,         // 触发「混天气功」概率<color=orange>#d1+%<color>
	magic_enhance_717_auto,         // 触发「两仪真气」概率<color=orange>#d1+%<color>
	magic_enhance_723_miss_p,       // 「魔音噬魄」恐惧敌人几率<color=orange>#d1+%<color>
	magic_no,                       // 未镶套
	magic_skill_collideevent,       // 碰撞子弹时
	magic_skill_vanishedevent,      // 子弹结束时
	magic_skill_startevent,         // 子弹开始时
	magic_skill_flyevent,		    // 子弹飞行时
	magic_block_rate,                          //档格概率
    magic_enhancehit_rate,                     //重击概率
	magic_anti_block_rate,                     //=拆解格挡概率：#d1+%
	magic_anti_enhancehit_rate,                //=抵挡重击概率：#d1+%
	magic_sorbdamage_p,                        //=抵消伤害:#d1+
	magic_anti_poisonres_p,                    //=忽略对方毒防:#d1-%
	magic_anti_fireres_p,                      //=忽略对方火防:#d1-%
	magic_anti_lightingres_p,                  //=忽略对方雷防:#d1-%
	magic_anti_physicsres_p,                   //=忽略对方普防:#d1-%
	magic_anti_coldres_p,                      //=忽略对方冰防:#d1-%
	magic_not_add_pkvalue_p,                   //=不增加PK值概率:#d1+%
	magic_add_boss_damage,                     //=对黄金boss攻击伤害<color=orange>#d1+%<color>
	magic_five_elements_enhance_v,             //=五行强化值：#d1-点。强化对相克五行的克制效果
    magic_five_elements_resist_v,              //=五行弱化值：#d1-点。弱化受相克五行的克制效果
	magic_skill_enhance,                       //=对攻击技能加成：#d1+%
	magic_anti_allres_p,                       //忽略对方所有抗性:#d1+%
	magic_add_alldamage_p,                     //加强伤害:#d1+%
	magic_auto_Revive_rate,                    //自动原地复活
	magic_addphysicsmagic_p,	               //内功铺百分比
	magic_addcreatnpc_v,	                   //抓捕BB
	magic_reduceskillcd1,                      //=减少技能#l1-冷却时间#f3-秒  （某技能的冷却时间）
	magic_reduceskillcd2,                      //=减少技能#l1-冷却时间#f3-秒
	magic_reduceskillcd3,                      //=减少技能#l1-冷却时间#f3-秒
	magic_clearallcd,                          //#d1,+%的几率清除自己所有技能冷却时间
	magic_addblockrate,                        //=周围每#d1+个人#d3+%格挡率
	magic_walkrunshadow,                       //=凌波微步
	magic_returnskill2enemy,                   //=被敌人攻击时有#d1+%几率使敌人被技能#l3-攻击  
	magic_manatoskill_enhance,                 //=满内力时对攻击技能加成：#d1+%
	magic_add_alldamage_v,                     //加深伤害点
	magic_addskilldamage7,                     //=增加「#l1」攻击伤害#d3-%
	magic_ignoreattacrating_v,                 //忽略对方命中率
	magic_alljihuo_v,                          //激活全身
	magic_addexp_v,                            //间隔多少秒 增加经验
    magic_doscript_v,                          //执行脚本状态
	//magic_addskillexp_v,                     //增加技能熟练度
    magic_me2metaldamage_p,                    //=对金系伤害增加：#d1+%
    magic_metal2medamage_p,                    //=减少来自金系的伤害：#d1-%
    magic_me2wooddamage_p,                     //=对木系伤害增加：#d1+%
    magic_wood2medamage_p,                     //=减少来自木系的伤害：#d1-%
    magic_me2waterdamage_p,                    //=对水系伤害增加：#d1+%
    magic_water2medamage_p,                    //=减少来自水系的伤害：#d1-%
    magic_me2firedamage_p,                     //=对火系伤害增加：#d1+%
    magic_fire2medamage_p,                     //=减少来自火系的伤害：#d1-%
    magic_me2earthdamage_p,                    //=对土系伤害增加：#d1+%
    magic_earth2medamage_p,                    //=减少来自土系的伤害：#d1-%
	magic_manareplenish_p,                     //内力恢复百分比
	magic_fasthitrecover_p,                    //自己受伤动作时间增加减少百分比
	magic_stuntrank_p,                         //眩晕几率减少百分比
	magic_sorbdamage_v,                        //抵消伤害百分比
	magic_creatstatus_v,
	magic_randmove,	                            //不能出招 以及随机巡逻
	magic_addbaopoisondmax_p,	                //增加爆毒上限
	magic_dupotion_v,	                        //药品的减毒状态 
	magic_npcallattackSpeed_v,                  //同伴攻速增加
	magic_eqaddskill_v,                         //装备了就增加什么技能
	magic_autodeathskill,
	magic_autorescueskill,                      //救命技能
	magic_staticmagicshield_p,					//昆仑120级技能魔法盾
	magic_ignorenegativestate_p,				//
	magic_poisonres_yan_p,                      //=毒防：#d1+%（阳）
    magic_fireres_yan_p,                        //=火防：#d1+%（阳）
	magic_lightingres_yan_p,                    //=雷防：#d1+%（阳）
	magic_physicsres_yan_p,                     //=普防：#d1+%（阳）
	magic_coldres_yan_p,                        //=冰防：#d1+%（阳）
	magic_lifemax_yan_v,                        //=生命最大值：#d1+点（阳）
	magic_lifemax_yan_p,                        //=生命最大值：#d1+%（阳）
	magic_manamax_yan_v,                        //=内力最大值：#d1+点（阳）
	magic_manamax_yan_p,                        //=内力最大值：#d1+%（阳）
	magic_sorbdamage_yan_p,                     //=抵消伤害：#d1+（阳）
	magic_fastwalkrun_yan_p,                    //=移动速度：#d1+%（阳）
	magic_attackspeed_yan_v,                    //=外功系武功出招动作：#d1+%（阳）
	magic_castspeed_yan_v,                      //=内功系武功出招动作：#d1+%（阳）
	magic_allres_yan_p,                         //=所有抗性：#d1+%（阳）
	magic_fasthitrecover_yan_v,                 //=受伤动作时间：#d1~（阳）
	magic_anti_physicsres_yan_p,                //=忽略对方普防：#d1-%（阳）
	magic_anti_poisonres_yan_p,                 //=忽略对方毒防：#d1-%（阳）
	magic_anti_coldres_yan_p,                   //=忽略对方冰防：#d1-%（阳）
	magic_anti_fireres_yan_p,                   //=忽略对方火防：#d1-%（阳）
	magic_anti_lightingres_yan_p,               //=忽略对方雷防：#d1-%（阳）
	magic_anti_allres_yan_p,                    //=忽略对方所有抗性：#d1-%（阳）
	magic_anti_sorbdamage_yan_p,                //=加深伤害：#d1+（阴阳）
	magic_anti_hitrecover,		                //=造成对方受伤动作时间：#d1+
	magic_do_hurt_p,                            //=造成受伤概率：#d1+%
	magic_skill_showevent,                      //飞行事件
	magic_addskillexp1,
	magic_anti_poisontimereduce_p,              //造成中毒时间增加  对敌人 
	magic_anti_stuntimereduce_p,                //造成眩晕时间增加  对敌人
	magic_addskilldamage8,
	magic_addskilldamage9,
	magic_addskilldamage10,
	magic_normal_end,
};

/*
poisonres_yan_p=毒防：#d1+%（阳）
fireres_yan_p=火防：#d1+%（阳）
lightingres_yan_p=雷防：#d1+%（阳）
physicsres_yan_p=普防：#d1+%（阳）
coldres_yan_p=冰防：#d1+%（阳）
lifemax_yan_v=生命最大值：#d1+点（阳）
lifemax_yan_p=生命最大值：#d1+%（阳）
manamax_yan_v=内力最大值：#d1+点（阳）
manamax_yan_p=内力最大值：#d1+%（阳）
sorbdamage_yan_p=抵消伤害：#d1+（阳）
fastwalkrun_yan_p=移动速度：#d1+%（阳）
attackspeed_yan_v=外功系武功出招动作：#d1+%（阳）
castspeed_yan_v=内功系武功出招动作：#d1+%（阳）
allres_yan_p=所有抗性：#d1+%（阳）
fasthitrecover_yan_v=受伤动作时间：#d1~（阳）
anti_physicsres_yan_p=忽略对方普防：#d1-%（阳）
anti_poisonres_yan_p=忽略对方毒防：#d1-%（阳）
anti_coldres_yan_p=忽略对方冰防：#d1-%（阳）
anti_fireres_yan_p=忽略对方火防：#d1-%（阳）
anti_lightingres_yan_p=忽略对方雷防：#d1-%（阳）
anti_allres_yan_p=忽略对方所有抗性：#d1-%（阳）
anti_sorbdamage_yan_p=加深伤害：#d1+（阴阳）






me2metaldamage_p=对金系伤害增加：#d1+%
metal2medamage_p=减少来自金系的伤害：#d1-%
me2wooddamage_p=对木系伤害增加：#d1+%
wood2medamage_p=减少来自木系的伤害：#d1-%
me2waterdamage_p=对水系伤害增加：#d1+%
water2medamage_p=减少来自水系的伤害：#d1-%
me2firedamage_p=对火系伤害增加：#d1+%
fire2medamage_p=减少来自火系的伤害：#d1-%
me2earthdamage_p=对土系伤害增加：#d1+%
earth2medamage_p=减少来自土系的伤害：#d1-%
*/
extern const char MAGIC_ATTRIB_STRING[magic_normal_end + 1][64] ; 


#endif
