function onaffect()
	local mid = ScriptCommonLib.GetString("modifierid");
	local sid = ScriptCommonLib.GetTempString("squadid");
	local eid = ScriptCommonLib.GetTempString("effectid");
	if mid ~= "" then
		SquadLib.RemoveModifier(sid, mid);
	end
	local lv = SquadLib.GetEffectLevel(sid, eid);
	mid = SquadLib.ApplyModifier(sid, 0, 0.0, 0.0, lv * 4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	ScriptCommonLib.SetString("modifierid", mid);
end

function onremove()
	local mid = ScriptCommonLib.GetString("modifierid");
	local sid = ScriptCommonLib.GetTempString("squadid");
	if mid ~= "" then
		SquadLib.RemoveModifier(sid, mid);
	end
end

function canaffect()
	ScriptCommonLib.SetTempInt("affect", 1);
end