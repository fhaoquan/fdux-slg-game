function useskill()
	attacker  = ScriptCommonLib.GetString("skillcaster");
	defender  = ScriptCommonLib.GetString("skilltarget");
	x ,y = ScriptCommonLib.GetSquadCoord(defender);
	SkillLib.Missile(attacker,x,y,1,"mp_flare_06","Skill","none",0,"none","none","none");
	re = SkillLib.MagicAttack(attacker,defender,15,25.0);
	if re>0 then
		ScriptCommonLib.SetInt("skillcast",1);
	end
end