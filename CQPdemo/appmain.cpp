/*
* CoolQ Demo for VC++ 
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "cqp.h"
#include "appmain.h" //应用AppID等信息，请正确填写，否则酷Q可能无法加载
#include <string>
#include <random>
#include <cstdlib>
#include <ctime>
#include <io.h>
#include <thread>
#include <chrono>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <utility>
#define DELAY(i) this_thread::sleep_for(chrono::duration<int>(i))
#define SIGN 1
#define RID 2
#define SEL 3
#define TALK 4
#define VOTE 5
#define GAP 6

using namespace std;

const int card_cnt = 394;
map <int64_t, int> player_cnt;
map <int64_t, int> game_status;
map <int64_t, int> game_round;
map <int64_t, int> card_sel;
map <int64_t, map<int, int64_t>> card_from;
//map <int64_t, int> player_status;
map <int64_t, vector<int64_t>> game_player;
map <int64_t, vector<int>> get_vote;
map <int64_t, vector<int>> player_card;
map <int64_t, int64_t> in_game;
map <int64_t, map<int, bool>> cards;
map <int64_t, string> game_rid;
map <int64_t, int> tmp_cnt;
map <int64_t, vector<int>> card_tmp;
map <int64_t, map <int, int>> card_ori;
map <int64_t, map <int, int>> card_num;
map <int64_t, map<int, int>> card_player;
map <int64_t, int> player_num;
map <int64_t, int> player_score;
map <int64_t, bool> player_done;
map <int64_t, bool> ans_right;
map <int64_t, bool> player_quit;
map <int64_t, int> quit_cnt;
map <int64_t, time_t> pre_sign;
map <int64_t, int> score_d;
map <int64_t, int> score_w;
map <int64_t, int> score_m;
map <int64_t, int> win_d;
map <int64_t, int> win_w;
map <int64_t, int> win_m;
map <int64_t, string> player_ID;
tm *pre_upd;

int ac = -1; //AuthCode 调用酷Q的方法时需要用到
bool enabled = false;


/* 
* 返回应用的ApiVer、Appid，打包后将不会调用
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}

string date_d(tm *now_time)
{
	ostringstream ss;
	ss << now_time->tm_year + 1900 
		<< setfill('0') << setw(2) << now_time->tm_mon + 1 
		<< setfill('0') << setw(2) << now_time->tm_mday;
	return ss.str();
}

string date_w(tm *now_time)
{
	ostringstream ss;
	ss << now_time->tm_year + 1900
		<< setfill('0') << setw(2) << now_time->tm_mon + 1
		<< setfill('0') << setw(2) << now_time->tm_mday - now_time->tm_wday;
	return ss.str();
}

string date_m(tm *now_time)
{
	ostringstream ss;
	ss << now_time->tm_year + 1900
		<< setfill('0') << setw(2) << now_time->tm_mon + 1;
	return ss.str();
}

void read_score()
{
	int64_t QQ;
	int Score,Win;
	score_d.clear(); score_d[0] = 0;
	score_w.clear(); score_w[0] = 0;
	score_m.clear(); score_m[0] = 0;
	win_d.clear(); win_d[0] = 0;
	win_w.clear(); win_w[0] = 0;
	win_m.clear(); win_m[0] = 0;
	time_t rawtime;
	time(&rawtime);
	tm *now_time = localtime(&rawtime);
	pre_upd = now_time;
	//printf("%d %d %d", now_time->tm_year + 1900, now_time->tm_mon + 1, now_time->tm_mday);
	ifstream ifs;
	ifs.open("C:\\Users\\Administrator\\Desktop\\CQP\\data\\score\\dixit\\day"+date_d(now_time));
	while (ifs && !ifs.eof())
	{
		ifs >> QQ >> Score >> Win;
		score_d[QQ] = Score;
		win_d[QQ] = Win;
	}
	ifs.close();
	ifs.open("C:\\Users\\Administrator\\Desktop\\CQP\\data\\score\\dixit\\week"+date_w(now_time));
	while (ifs && !ifs.eof())
	{
		ifs >> QQ >> Score >> Win;
		score_w[QQ] = Score;
		win_w[QQ] = Win;
	}
	ifs.close();
	ifs.open("C:\\Users\\Administrator\\Desktop\\CQP\\data\\score\\dixit\\mon"+date_m(now_time));
	while (ifs && !ifs.eof())
	{
		ifs >> QQ >> Score >> Win;
		score_m[QQ] = Score;
		win_m[QQ] = Win;
	}
	ifs.close();
}

void write_score()
{
	time_t rawtime;
	time(&rawtime);
	tm *now_time = localtime(&rawtime);
	ofstream ofs;
	map <int64_t, int>::iterator it;
	ofs.open("C:\\Users\\Administrator\\Desktop\\CQP\\data\\score\\dixit\\day"+date_d(now_time));
	for (it = score_d.begin(); it != score_d.end(); it++)
	{
		ofs << it->first << " " << it->second << " " << win_d[it->first] << "\n";
	}
	ofs.close();
	ofs.open("C:\\Users\\Administrator\\Desktop\\CQP\\data\\score\\dixit\\week"+date_w(now_time));
	for (it = score_w.begin(); it != score_w.end(); it++)
	{
		ofs << it->first << " " << it->second << " " << win_w[it->first] << "\n";
	}
	ofs.close();
	ofs.open("C:\\Users\\Administrator\\Desktop\\CQP\\data\\score\\dixit\\mon"+date_m(now_time));
	for (it = score_m.begin(); it != score_m.end(); it++)
	{
		ofs << it->first << " " << it->second << " " << win_m[it->first] << "\n";
	}
	ofs.close();
}

bool cmp(pair<int64_t, int> p1, pair<int64_t, int> p2)
{
	return p1.second > p2.second;
}

void show_score(int num,int64_t Group)
{
	time_t rawtime;
	time(&rawtime);
	tm *now_time = localtime(&rawtime);
	map <int64_t, int> *mp1, *mp2;
	map <int64_t, int>::iterator it;
	ostringstream ss;
	int tmp;
	if (num == 1)
	{
		mp1 = &score_d; mp2 = &win_d;
		ss << "【Dixit 日榜】 (" + date_d(now_time) + ")\n";
	}
	if (num == 2)
	{
		mp1 = &score_w; mp2 = &win_w;
		ss << "【Dixit 周榜】 (" + date_w(now_time) + ")\n";
	}
	if (num == 3)
	{
		mp1 = &score_m; mp2 = &win_m;
		ss << "【Dixit 月榜】 (" + date_m(now_time) + ")\n";
	}
	ss << "得分Top5：\n";
	vector <pair<int64_t,int>> Score;
	for (it = mp1->begin(); it != mp1->end(); it++)
	{
		Score.push_back(make_pair(it->first,it->second));
	}
	sort(Score.begin(), Score.end(), cmp);
	for (int i = 0; i < 5 || (i < Score.size() && Score[i].second && Score[i].second==Score[i-1].second); i++)
	{
		if (i == 0 || (i < Score.size() && Score[i].second != Score[i - 1].second)) tmp = i + 1;
		ss << tmp << ". ";
		if (i < Score.size() && Score[i].second) ss << Score[i].first << " " << Score[i].second;
		ss << "\n";
	}
	ss << "获胜数Top5：\n";
	vector <pair<int64_t, int>> Win;
	for (it = mp2->begin(); it != mp2->end(); it++)
	{
		Win.push_back(make_pair(it->first, it->second));
	}
	sort(Win.begin(), Win.end(), cmp);
	for (int i = 0; i < 5 || (i < Win.size() && Win[i].second && Win[i].second == Win[i - 1].second); i++)
	{
		if (i == 0 || (i < Win.size() && Win[i].second != Win[i - 1].second)) tmp = i + 1;
		ss << tmp << ". ";
		if (i < Score.size() && Win[i].second) ss << Win[i].first << " " << Win[i].second;
		ss << "\n";
	}
	if (num == 1)
	{
		ss << "数据每日重置";
	}
	if (num == 2)
	{
		ss << "数据每周日重置";
	}
	if (num == 3)
	{
		ss << "数据每月一日重置";
	}
	string s_tmp = ss.str();
	CQ_sendGroupMsg(ac, Group, s_tmp.c_str());
}

/* 
* 接收应用AuthCode，酷Q读取应用信息后，如果接受该应用，将会调用这个函数并传递AuthCode。
* 不要在本函数处理其他任何代码，以免发生异常情况。如需执行初始化代码请在Startup事件中执行（Type=1001）。
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 酷Q启动
* 无论本应用是否被启用，本函数都会在酷Q启动后执行一次，请在这里执行应用初始化代码。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventStartup, 0)() {
	return 0;
}


/*
* Type=1002 酷Q退出
* 无论本应用是否被启用，本函数都会在酷Q退出前执行一次，请在这里执行插件关闭代码。
* 本函数调用完毕后，酷Q将很快关闭，请不要再通过线程等方式执行其他代码。
*/
CQEVENT(int32_t, __eventExit, 0)() {
	write_score();
	return 0;
}

/*
* Type=1003 应用已被启用
* 当应用被启用后，将收到此事件。
* 如果酷Q载入时应用已被启用，则在_eventStartup(Type=1001,酷Q启动)被调用后，本函数也将被调用一次。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	read_score();
	return 0;
}


/*
* Type=1004 应用将被停用
* 当应用被停用前，将收到此事件。
* 如果酷Q载入时应用已被停用，则本函数*不会*被调用。
* 无论本应用是否被启用，酷Q关闭前本函数都*不会*被调用。
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}

string URL(int i)
{
	switch (i)
	{
		case 1: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/sX*onP2CjwcyM0TDAKhMrmmE1KgD4StITIW73ANofWQ!/r/dDUBAAAAAAAA";
		case 2: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/hx6OmdyDGFtYvZAT9WevOXAc.LCxx6riXJ*J410p6Ro!/r/dFIBAAAAAAAA";
		case 3: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/PTL7ooP8xFKkFYDs7lZ8TF*Lfe5Tif7DBbA61tdqTBo!/r/dDcBAAAAAAAA";
		case 4: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/omfmadEqGsTpayOdfuGgzxHVXdh404dSmLaqji.bYjo!/r/dFMBAAAAAAAA";
		case 5: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/TJRvJc9gKQt5Kp.Q2zybysngHem3rQqHyGHF.lSFFBw!/r/dFMBAAAAAAAA";
		case 6: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/8x9M0n.HoDEFktJOpdw3XNqeSM6*L3Pz.CAXG7PRvug!/r/dFUAAAAAAAAA";
		case 7: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/EtI29i4L1ixd5Al3oN8bNr7y5kvU*OEDU1pzRl0o8o0!/r/dDYBAAAAAAAA";
		case 8: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/BCSRrUcBA3O.H9nCAzGDI7XTTrae6En6HHuyRONH.EU!/r/dDEBAAAAAAAA";
		case 9: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Jt9Dl9Gim04jjP4BhqhcVsFIP8hDkHbVhXgKpkaDMCg!/r/dDcBAAAAAAAA";
		case 10: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/NSp6mFi4Z7s3EXiOBowjyagNmxGDCHlbcTmGq59cvnc!/r/dFQBAAAAAAAA";
		case 11: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/uuUtpDTJWZ6FEUEqHZRPq8oGEqF*KoaPhDWkJfzYGpY!/r/dDcBAAAAAAAA";
		case 12: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/3H5fHKB3EestA5BXwielIlp2qMbKg2qkMV8Ox4dP5mc!/r/dDEBAAAAAAAA";
		case 13: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/KUInN1VSDLmd4V5rUav4GmIiSwy5pducXVy0*TuWI7M!/r/dEgBAAAAAAAA";
		case 14: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/G57UhpEw0pjwD.ttn16rnOxCCogXwKxJHpOsdVpVF88!/r/dDYBAAAAAAAA";
		case 15: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/CGV5s2B55DSo07IU*3SLS5fNE6WfstYtyYsBL2Jv1Zw!/r/dFMBAAAAAAAA";
		case 16: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/32Hc*uOMyLum2peeN.sJ52cd5d0eccjfmj26Odl3Q7A!/r/dDQBAAAAAAAA";
		case 17: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/VQoE74wa5h.5e8Nseu4hTUpbppxFsqvkptTLsTBDMd0!/r/dFMBAAAAAAAA";
		case 18: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/WfgSyUESVT0SyavLa6nJ9p7K3RwAqow0ERcQxj7oHQk!/r/dEkBAAAAAAAA";
		case 19: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/MImRVC7PyQ8gfTyIjXWwbad5WE7vZZpUgV9RqILHpD4!/r/dFMBAAAAAAAA";
		case 20: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/bxai0WsyiY3HcUC422JgnjlPwTuCXsxg.uOozuUdY94!/r/dDUBAAAAAAAA";
		case 21: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/1YA*aT*Qlj7wfIZjUhh1.HYVUm6qFV4uulKbmiiekcc!/r/dDYBAAAAAAAA";
		case 22: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Z2cpFyb8KDHYzGR0.nUO63zWCahO0zkJf7W1ONGM.FA!/r/dD4BAAAAAAAA";
		case 23: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/VX.NnFU*Qgi7Ix7yCBJbb9tKqNGEEDYBBhlZGO*f0HQ!/r/dDYBAAAAAAAA";
		case 24: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/HmlA104BRYFWTToMmvupi8c*QY3S153Lgx.KPZSWWvw!/r/dFIBAAAAAAAA";
		case 25: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/xFm0A.moQuUQaJVgCqD8LQHVYcJ9Go1vy070TmfkhDg!/r/dDQBAAAAAAAA";
		case 26: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/1tIsfaunTGOQXOWsVhmBdFxVx67ohD71XSKQXW5Ua4s!/r/dFMBAAAAAAAA";
		case 27: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/4ws0tLCL.Lw*ZcRqzigNLzLHQdEE88rO7KRj.LSi**k!/r/dFQBAAAAAAAA";
		case 28: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/bhGZfOA6WGyncU3Ryy10ySScN7GnJlGjdHCiuc9UNJE!/r/dDEBAAAAAAAA";
		case 29: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/50JWry6DDEEqdtUOuMqB.IGpIRFWI4LZSSaGPYZ6J4o!/r/dFQBAAAAAAAA";
		case 30: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/vAlCjzZcFBJYELsiiJr5gdBQHr6znnToadhIvLy30.k!/r/dDcBAAAAAAAA";
		case 31: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/L4BFan0QC9E.fMdElbMwUB5vKt845m.XCPiH4hRNoUc!/r/dEgBAAAAAAAA";
		case 32: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Db1PU5VhcNiEle1B5s4t*gSBp0eZkVHShlPEJNUJNQ4!/r/dDUBAAAAAAAA";
		case 33: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Re0Mgbf9vt0qb9i64jJSShlRyCI730BWrR36.4AA*Xw!/r/dFMBAAAAAAAA";
		case 34: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/DownUZ7MxFyQDtGbfEIUc0E*dG68cEEFJbesaTMsn6Q!/r/dD4BAAAAAAAA";
		case 35: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/vJZRYjurheCBvmTO3t5Q2DTUSghA5NrkmxHSEvEaq7U!/r/dFQBAAAAAAAA";
		case 36: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/YIhPV1SoyvgPPw.EguCan2zGjUJWq0qFPdC2oiFhJWs!/r/dEYBAAAAAAAA";
		case 37: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/m6k1vhqdsCVyy7ocLNqBjcV1OUNLEDY0EVA7pXE4fN4!/r/dFIBAAAAAAAA";
		case 38: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/o0gHd2mVuvdBlyCvR6iCYJkCHXIYkXnCugLii7pyBO4!/r/dFMBAAAAAAAA";
		case 39: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/EWgXxotabRwnXcuMGGcW5byVSxhH1h0Paaxa2Dciq6A!/r/dDABAAAAAAAA";
		case 40: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/UOe88cUYuuCucctWP.zTVCGK8atwFp8HWzxqL4KeVeE!/r/dDEBAAAAAAAA";
		case 41: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/GBBuejKl4sD5BviBLANb7w0H.yLCePLB56Y8JSjZD2A!/r/dFIBAAAAAAAA";
		case 42: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/OZOl0u4feBusUDTEhuGHxyABik3oajTukZA6tW74cQA!/r/dDUBAAAAAAAA";
		case 43: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/HqWT.9FplENxnVikR8I*jrhhLHxrv9f18oTt*zQ.j7g!/r/dFQBAAAAAAAA";
		case 44: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/wZd370a4MziyHGWSOC5La0V9eOrAJsKhGJpM.WdHEZ0!/r/dDUBAAAAAAAA";
		case 45: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/a6FFcuHHDmS8aeX06YOwP7D9zjpGFxVvn5xtQyPKBvk!/r/dDABAAAAAAAA";
		case 46: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/kNXZqVFYDsSIWDFM4hOUfWswuUsFpdj7FPsC9apy6h0!/r/dDUBAAAAAAAA";
		case 47: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/h6YWN2vSrMrc4o4TwmptG58zCvHe5*kbhKGSj4pp.6I!/r/dFMBAAAAAAAA";
		case 48: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/WMpkLYD7ZdfaVEqAam5nDiYQ4EDRyx5aiPzHnH9qINE!/r/dFMBAAAAAAAA";
		case 49: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/kJScK7il3rN.CaAQ3TGRJ9F8NUXZ01u9WvntvUwNTPg!/r/dFMBAAAAAAAA";
		case 50: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/iQFVcHchCKACt9s4PNKC09Jc5txc0QthIDA8q5wB9aA!/r/dDQBAAAAAAAA";
		case 51: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/kTygh5tTxLpPTSEtNxflgJS7KhAstTR8CAH97dW8JME!/r/dD4BAAAAAAAA";
		case 52: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/UycgbjCgyi2hqhUSKSyZmohYnHCvx6mT8UsvFW2DJ8Q!/r/dDIBAAAAAAAA";
		case 53: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/O29.mWPzQLPNpT.WwU1rX2QHIQ65Ml*7bjQnFMTyNMc!/r/dFMBAAAAAAAA";
		case 54: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/hRuYOF.7GRbOvFfa41.j77hDX9qCX76CioEGNP5dZZM!/r/dFMBAAAAAAAA";
		case 55: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Ae5YYxgjlot0nH08cD6T6uc3ChZwXBxRSAYArrN6GmI!/r/dEkBAAAAAAAA";
		case 56: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/C.5Md*6Hw6sKE*mu1ZbQVSlouufBSf2LY.ZRSZTCJ8A!/r/dDQBAAAAAAAA";
		case 57: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/A3uLXpZezhOuQgmVAdkzBoPRlGryOXtoEFme7pxcvU4!/r/dFIBAAAAAAAA";
		case 58: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/SM2*.ltOvtgR3TZC45kUFOxkTHQaTq1taJERFGwDc4g!/r/dFIBAAAAAAAA";
		case 59: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/47n5UtbW3sUyO6wfh1soe8j5S7fi9XIpnTgLvOhOmw0!/r/dFQBAAAAAAAA";
		case 60: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/m1LtCa0zBvTI.XAcnDHS95o9rTq83Qa6fhs3xmyqskA!/r/dFMBAAAAAAAA";
		case 61: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/JWEzftWQIRKlr3OALc7jjnCGq9TJolSL9QQR6nCV6T0!/r/dDYBAAAAAAAA";
		case 62: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/TlGJ*ub5H.brsf3ZeVjTjoHaE21awEKQ02Jm2IaCtCU!/r/dDQBAAAAAAAA";
		case 63: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/ci2pmQeOsZ8BQP54r*8Mnty0lK0KUJvpUMESr3QeHbg!/r/dDMBAAAAAAAA";
		case 64: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/EdLBIfF6J58S4bEvbkuPJGMyEYN.gziISae95MDI1dU!/r/dDMBAAAAAAAA";
		case 65: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/exsJkGfoD6r93G9izAOQzIsqwBrn*j1adGjqDF.viiw!/r/dDYBAAAAAAAA";
		case 66: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/72HqK1jObstohANk2PrqJgCum8KXThH0kMN6wEn9qj0!/r/dDcBAAAAAAAA";
		case 67: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/zI4j6.fleG4Ikhr6OHDlTN3HmSYZ0DHzje9tk3*8A5U!/r/dDYBAAAAAAAA";
		case 68: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/wMq*qFslVyZuAADLZTisBtr.m9r2epVIxpDjU*BDBBw!/r/dDABAAAAAAAA";
		case 69: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Qt*e*uJ.BOJjnLUnrg71DualtsSNEIrFR*FWaxeL4uU!/r/dFQBAAAAAAAA";
		case 70: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/JiXB92itlobhKO7AMb75hH5.Ysd1arX6I5pIXtlW2Fg!/r/dFQBAAAAAAAA";
		case 71: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/S09MtSet6yNyt0ZytTkQC6ahpS0rMY40RVH1scS*40Q!/r/dFQBAAAAAAAA";
		case 72: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/A7LXB3FMN4Pig7kc8FnqdBsJlSqjHTuclkNuxkUpplE!/r/dDUBAAAAAAAA";
		case 73: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/t5fniqyeKj7lP*1QEcqTvW.1ueytGk*QBsl9lMwJfB0!/r/dDMBAAAAAAAA";
		case 74: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/opaHwm622aDxZRxRM9IXYjJfb*YTDvJlEQ9FErrx2sM!/r/dFIBAAAAAAAA";
		case 75: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/ox0LqbrSkPXHDK6ff7ewxZDSarNAkUr8hYkRK7ozHv8!/r/dDUBAAAAAAAA";
		case 76: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/k8GqnMv99ukH8ZHNZkKg7JwlPy36gdAj7i68*3sV62Q!/r/dDcBAAAAAAAA";
		case 77: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/.9Q2KZuQAe30.aQ*sgbPJBjWp67qPvp031kmmZsJZxI!/r/dFMBAAAAAAAA";
		case 78: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/v0T2o6MAOzVUDaYPyIWGbce6tnOrOAwCXZhAfIS4oVU!/r/dDYBAAAAAAAA";
		case 80: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/vZWRLnQHqUpTt*OjMGCNq4B7g8Z20e4ZC54DAkjiq*k!/r/dEgBAAAAAAAA";
		case 81: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/oOoZtFF8QF0DAtiTUBx0jUV7H*IfzCFoPaA1iy8RWb8!/r/dDABAAAAAAAA";
		case 82: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/PYRuYuC.u4W0s01J.4.pNkPjIEQ0MesjKJyJGSU1yok!/r/dDUBAAAAAAAA";
		case 83: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/R5PJXxhABgmy5i40TmBShG7*kgx8L*XDAcptoxqmZc4!/r/dDUBAAAAAAAA";
		case 84: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/JV9.eB0sG6c*zvfyYKwBlcuu9B3o3T8890h.0THuWpw!/r/dFMBAAAAAAAA";
		case 85: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/HxtwDtxYuompbTDt8hhp82*IwN2583nOVur1P9z98iQ!/r/dAgBAAAAAAAA";
		case 86: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/7R207ep60ArK4FHkU4pl*DE7jdMDqsFadFp6U1jmYGY!/r/dDUBAAAAAAAA";
		case 87: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/sW5sr3SM.1u6sVq7T4a9IPyv3D3MVwaGeQyhl*al7po!/r/dDUBAAAAAAAA";
		case 88: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/4m68tGYXp8O8RB.7da.js.M6qwnwcG2JadP9kLY7FBc!/r/dD4BAAAAAAAA";
		case 89: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/rp5hZ56JKnBVgFmwaO0HHhlsaouFe5VzF1lWTrYHMc8!/r/dDcBAAAAAAAA";
		case 90: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/uuycr6aHLdTzbkm5*pOuxHoe6JuDNgB9jgM*.aRmiM4!/r/dDUBAAAAAAAA";
		case 91: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/0vc4366UibxMrI4VyPB0gL.ZSV2XJ2*OsA9tsJ54nSs!/r/dFMBAAAAAAAA";
		case 92: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/2bpGDCLcmZoL*J8wESwCE*r0eFXczBpRrV7kJDZ9JGU!/r/dDcBAAAAAAAA";
		case 93: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/x15cHIEtGrIUj1SaEwvzDteDHSK4dqG*CrcNYDXb3yk!/r/dFIBAAAAAAAA";
		case 94: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/9POwxl47zlH3mzMbmjG8DEPDWjf9crHDTBtt4.HioNA!/r/dDABAAAAAAAA";
		case 95: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/VrhKyuD0dd*tyWt1ImkLFP1PrBA9PqK5IkB8l7HuvaM!/r/dFQBAAAAAAAA";
		case 96: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/6dJbUrBG4R1O7uAzLQ5OO8eUk9BXGuLGfl8hrqsbbHU!/r/dDMBAAAAAAAA";
		case 97: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Z6rCFCv5ufxvT9zOaOrc03CAVtCLXTAQ5XR9olQG0F8!/r/dEYBAAAAAAAA";
		case 98: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/W3JSvfpj8hRtvsA6acpuaXw6XIHBie7wPiN8M27bAvk!/r/dFMBAAAAAAAA";
		case 99: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/jRw2ZhHS.Tge*RxeJO4wH6*2WumqVHdCoHVZ4mp6Vic!/r/dEcBAAAAAAAA";
		case 100: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/mrB5YtWaw8KaMVmKsJ77owMQfPe5*EmFaIVgQfPwf90!/r/dDQBAAAAAAAA";
		case 101: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/CfqgLsKhe2.LM5GUC*sTWx4b12rOSLV2YgyfMQZ1pvk!/r/dFIBAAAAAAAA";
		case 102: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/9hJV8GoiL4t6GcWuWv0dmPEu8VZdwQ3.FMBeb.OWFvA!/r/dDQBAAAAAAAA";
		case 103: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/7CcCtd*Fz2N0HBzQEM3HvSBRnnoLCg7oknY4.sf52GY!/r/dFIBAAAAAAAA";
		case 104: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/B.MP3IVmuLDQgnUl0Ye7p5eXbGrrpZZChMLDJEDAwFM!/r/dFQBAAAAAAAA";
		case 105: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/d3oOrOtpLGAUKkzvyNUPgN20MS1rxWjRaxIBFkPma*8!/r/dDcBAAAAAAAA";
		case 106: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/DrWtLIehxXv6.ApmL*AUEGbqR8ryiTZyYRLEmpZzk5E!/r/dDQBAAAAAAAA";
		case 107: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/0.fnWnI2g*KE9Uz2wPbOJothZQNatYsDjmtS1Le024k!/r/dDIBAAAAAAAA";
		case 108: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/JYs1h7IaxzQN*oR6CjE3OCjn5..3eYBQMlUE*AkS5V4!/r/dFIBAAAAAAAA";
		case 109: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/DTcsDDoDgCgr9F2x1WYL2MdW9BFoFkaBydWeVqRcaRI!/r/dDUBAAAAAAAA";
		case 110: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/a35xay6rp8X*jl5VV3.AjeUy6OBMu4KU9olpskcR.3s!/r/dDMBAAAAAAAA";
		case 111: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/g28UxxwwiOQ1lO0Z8qJbWAEVpKhxDEWW.YhSukxiBPM!/r/dFIBAAAAAAAA";
		case 112: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/vxTpk9P1DN1fzw6bpfn.zW.6FrKo6PgeUpCfmCCNTnc!/r/dDUBAAAAAAAA";
		case 113: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/k9iNkRDqh4Sc6sRl7JkOT4HgrZa2rOfdEMlzbJoH*4U!/r/dFQBAAAAAAAA";
		case 114: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/DPM4XRB3J9.dlySQS2g5H.Vq3GVs9yh7irz2ku7v14I!/r/dDYBAAAAAAAA";
		case 115: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/IQ1Hnr1XxNrdNFL0AayrBPxA2jjLQtjlAv9EMbOp9i4!/r/dFMBAAAAAAAA";
		case 116: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/JHV9vr1*xAm7LO8k5mUyuAFX2D3yxLNik9YHtHc99TA!/r/dFMBAAAAAAAA";
		case 117: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Snx3SjITDWj6iNbeEw5GKYAm8rC9g3w5B9TaWQL6*A4!/r/dDEBAAAAAAAA";
		case 118: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/kudJnBqCwWsy5EOxk8yKC*L32YWzesCFZMqud1TrY8Q!/r/dFIBAAAAAAAA";
		case 119: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/ltL*M2dKkWj7zzbWvnNoerJzFLXY2NF1JWteEx1IuNc!/r/dFYBAAAAAAAA";
		case 120: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/iGb7JSR3wDRV67wWR2ctRuAsV*l2LckxoKnobsWFfGU!/r/dDUBAAAAAAAA";
		case 121: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/04sfVM0Z0kOwV3rtiVPzyRyoeXVsaQQBP9Z3*mb9mbA!/r/dEYBAAAAAAAA";
		case 122: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/07ZioQ0gO08VZnAvOfnHPNjp1RHMO2C2BFVGqOTIDGg!/r/dFMBAAAAAAAA";
		case 123: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/qfg*nJER5g2xuP3iti2e.DqrWumuAA6a7zBV1kJGNZo!/r/dDYBAAAAAAAA";
		case 124: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/4hj*7ew7iU.EKyAobhZbV73r*Petx6eb07InUgZc8ks!/r/dC4BAAAAAAAA";
		case 125: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/9KJLuOjHr8VsuRA9CePoZHssIXiK*RnU*z8RfLoGGx0!/r/dDYBAAAAAAAA";
		case 126: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/mMVlw7p7b.dbznNcGvGYGXKo6.Lrj.6myvP2D8ZRLD4!/r/dFIBAAAAAAAA";
		case 127: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/vCnx3RQ6kckp2dfJsMEJ149LemE4GxhkNTKzZONM0mo!/r/dDUBAAAAAAAA";
		case 128: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/*mX*KmrBzNw6tM1UlStionGrwsgpDy*hB.G1CsYUOLg!/r/dDIBAAAAAAAA";
		case 129: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/mE82k9QOwrojKdxvJU9Im7sd5Ze4vS1qr.u8qB62h2k!/r/dFIBAAAAAAAA";
		case 130: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/2kSXZKAZzCJRh5NHmdlqmSt1scZqeEtJEkMfOtLGfJA!/r/dFIBAAAAAAAA";
		case 131: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/*CQu3Mn41qQceIKQO3wxNh3D7gyMv5KxeacqA7mFPpg!/r/dDABAAAAAAAA";
		case 132: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/6wqjNHmew6gc42TJqaa2qwvo1Ziq.wWu7fbcWf9E*38!/r/dDEBAAAAAAAA";
		case 133: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/AV2DG7CNEiMc9rq4PQ9LeTpToMPcPPkFxlmKPmW6a.4!/r/dDEBAAAAAAAA";
		case 134: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/XAwtBv8GUhNfaHp3XengVV9hGHJTsa*oRh9JPcJ3*rE!/r/dDcBAAAAAAAA";
		case 135: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/b6B.TrsX6BYpqSS09.yeljJIkFV1GJz0kWOM4HwXDC4!/r/dDYBAAAAAAAA";
		case 136: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/MprhvL81kva3B47Z8Z1.6aa*Nwf9hKO9KCaePyVtRjk!/r/dFMBAAAAAAAA";
		case 137: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/H5yIen7GLPaUGuAcVWmyF*BVs72nyGcDyqrkhH6hw30!/r/dFIBAAAAAAAA";
		case 138: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/HvcKMXNiqX.qxA0Hc2vmNFj.vFqLxN*mAnC.Qw2245o!/r/dFMBAAAAAAAA";
		case 139: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/q3dghyqLF5LLAohYKa5Lzxc6*7QOlFe3XaYe*Wbklxg!/r/dDMBAAAAAAAA";
		case 140: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/VFizM63yA1G84J5jjVQJfemWLdjkNaddwKjmx6oeo3s!/r/dFMBAAAAAAAA";
		case 141: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/0jZiemGnui*vqtM05AhKf9CV*Vb5jVkykUlZRmYn2jM!/r/dC8BAAAAAAAA";
		case 142: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/acsyaeCb5mSsjFlYpK02e2DGo.d9QJFKfyI2ZP5GKBA!/r/dFUAAAAAAAAA";
		case 143: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/cfY7pFYWTuFbWnWWSMeQIVtHbxSUr4OGNcgS.GMgRGE!/r/dDYBAAAAAAAA";
		case 144: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/sSA.JHsY.pwjGq3E7O8vd73FSKpBeV60bl6eH3paR2g!/r/dDYBAAAAAAAA";
		case 145: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/xWQtb6Nvrdtm6BW.AA52p57A8TIa0RulwcDHltidVso!/r/dDQBAAAAAAAA";
		case 146: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/.i.vC6AtX5*EqvffMxMQU7mal.pzBdTztbj.qfOuiP0!/r/dDQBAAAAAAAA";
		case 147: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Pl51MDr7Kd.C544D1C3Gxu1dXQ38ic4kZIVtIlQAl60!/r/dEgBAAAAAAAA";
		case 148: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/7NHhSfth3hN5Q*Zxo1Rxn1.bsmgXhlBKe0uRwH6o8Sw!/r/dDMBAAAAAAAA";
		case 149: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/.Tr*EKphmGU83fd0mLIarEN.4aDy5sJi51IeICM4eYE!/r/dFIBAAAAAAAA";
		case 150: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/uMKZPxGKwyseLIVbPP3jJYa6EVBwc7glupMcF1Qk.HQ!/r/dFYBAAAAAAAA";
		case 151: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/PVE6w53Wb6GrTXsxgmR0V.k7sUW5D7G1*fngmiyoN9g!/r/dFQBAAAAAAAA";
		case 152: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/JlRSpMr8tXKrUzvULhm9nthyNDG7pNa*oxvxmzElT40!/r/dFMBAAAAAAAA";
		case 153: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/1Sdhbjl0xa8C8c43OJSjgDHUp*vLq.tf0WVm0hK.4Fs!/r/dDQBAAAAAAAA";
		case 154: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/sCqaMeeD4p4lZ2wcstHD8sGztMV5ZEcCpczMYV0tu1E!/r/dC0BAAAAAAAA";
		case 155: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/1SXYK6ewPlHBvPs1lKDXEm59prb1V*a4zvswddlFvw0!/r/dDcBAAAAAAAA";
		case 156: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/qwwO3kCi.BwwMv3*i6A5XB9Ru*Z3FeH5uka6D*XDfew!/r/dFUAAAAAAAAA";
		case 157: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/KDP6P5P4ZNLkh5uTudVhFV83TuczZtZV76z1lxyk2UA!/r/dFMBAAAAAAAA";
		case 158: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/pZqAb8Gn9K1XSxyB5xRkfRAD5jXXhvPv48IGATXGcjM!/r/dPMAAAAAAAAA";
		case 159: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/1L.Awy3L989JT3L8dZWwmGvzWFwe5z7PFcua9rXI4*4!/r/dDYBAAAAAAAA";
		case 160: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/MD4cbHVPFyFW5nLYGDCA50ttWZu8v0CMzGYHbMhU2X0!/r/dDIBAAAAAAAA";
		case 161: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/qNSGRpTU5od0yJoC8VMruJhGl2kUfUGcExUct74yWAU!/r/dFMBAAAAAAAA";
		case 162: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/2X89tsmLyitdDy1fPLnE5FYqSOHvMoB6chENuplyJX8!/r/dFQBAAAAAAAA";
		case 163: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/HV5wiPOgI2lF6VL83Pa0*tRFplWqSARBOxvSS0XgRiI!/r/dDYBAAAAAAAA";
		case 164: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/zdby*nqU0WawM1SPzsxvOD3zCwNcTxZbG0M*Jz0SJeM!/r/dC8BAAAAAAAA";
		case 165: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/mfaCcqdULexsYHsJYQLFFUTTtJvXMRNZp86*yfZ9RIk!/r/dFQBAAAAAAAA";
		case 166: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/oDkbZq8jRVhuLory1lB0SJMTOi79mWWk5AUdWCGQdbs!/r/dDQBAAAAAAAA";
		case 167: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/TjRMMqlHLPGBbIO4bc.0akeFFEnms1sKfvaQXIIDOSw!/r/dDYBAAAAAAAA";
		case 168: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/iPuXWtsfjGyi5kJNarrEgZYvU5DWi*KNP67IPJDKFvA!/r/dDUBAAAAAAAA";
		case 169: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/swER7znodZOR.S5DhfP2V16l7PENYSjU7GGMBgVhpww!/r/dFQBAAAAAAAA";
		case 170: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/uvp07TJVK7*xYHGESHh3X8SJX2zQqRK7HahI5.bxXO8!/r/dFQBAAAAAAAA";
		case 171: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/fc9gegNL3gIoKFyWb4LzEmBaNbNr5TQLyUh0KXulkHY!/r/dFIBAAAAAAAA";
		case 172: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Dq.lfyDvH9c828bGq2ERlvE*2SBcTqewYbViEqNg*4s!/r/dDQBAAAAAAAA";
		case 173: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/*20KR2ba1byZfdFoWwdIhr19w8REzFhSX514BL0V5AM!/r/dFQBAAAAAAAA";
		case 174: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/UDtnn1NFDPB53NNVjgpDpI3P4K1whKbulig20IP9Nz8!/r/dFMBAAAAAAAA";
		case 175: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/vbeN4a4ratf7Zo2BHVGRbfU.yoFCB4.cgiuQiGtsXz4!/r/dFIBAAAAAAAA";
		case 176: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/u3qEG5.6.lhCjbvjrbiNT8jNVaqoe.3xWR27dFv*EYo!/r/dDUBAAAAAAAA";
		case 177: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/vDv7jciPdbOPIoIJaE05Wv2llfgf9y6KasEFGVZWAuQ!/r/dDYBAAAAAAAA";
		case 178: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/tZfrv.VDNeTgQ4RXAH0byta0rGsUuy9e7wNwU7gTwH8!/r/dFIBAAAAAAAA";
		case 179: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/3QzsYoOY9LUSsvX1.EzceDA7D*Boc8YiudcaqKhTTi0!/r/dFIBAAAAAAAA";
		case 180: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/48Nk822mBpMdG*xWzUh07V79gKHuOn2666dm1x5us0k!/r/dDQBAAAAAAAA";
		case 181: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/jJaCV7faQ.zHQN2USxH8qQb7wcK8gyfzINXrcmYSNVY!/r/dDUBAAAAAAAA";
		case 182: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/I9jzJ0qhCFS3fdtoAOPbcajPx6cAIe7EoyKYjyfISCk!/r/dFIBAAAAAAAA";
		case 183: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/IyWbwfTHj*N0vK3Tqx*JI551L3Sp7LkXZFkWRN6pHzI!/r/dFQBAAAAAAAA";
		case 184: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/vbjNyWik0ghQZl*92yMI.2Qa.q45dxTVYDLl*cNjZkQ!/r/dDYBAAAAAAAA";
		case 185: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/F5DtBvaEjcrtz54KEGeMfXe4oRpv56UlSBSqauXVmsY!/r/dEcBAAAAAAAA";
		case 186: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/QcXn8Oi6Jd1Bl65c1zSIvOL3kKY53g7.aWoaoHLv*oA!/r/dEcBAAAAAAAA";
		case 187: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Capczs7Ji*9J4bk7nmlnsKxHHkjBLTSoerkflc*fWjM!/r/dFYBAAAAAAAA";
		case 188: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/swQIdQbBWlFZcqOsilZybicaKFtAIrlJKAmOm43X*hE!/r/dDUBAAAAAAAA";
		case 189: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/cHhwo1Ts5R.VRFGnXJrOUByR3oiIBsuYHzMfMbzP.6M!/r/dDABAAAAAAAA";
		case 190: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/CgLatkU2mXkrEJSsqihm2EVpt24M4O*8K35rkhUNK58!/r/dDQBAAAAAAAA";
		case 191: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/2PfWobHHJoUoevsVWJbIAU1UQXysYk62WlEhXjWUl8A!/r/dFYBAAAAAAAA";
		case 192: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/jTFqXzhdKnekujYvCoepTOVkqObXoca9tj5BR27rYlw!/r/dDQBAAAAAAAA";
		case 193: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/*6ecPd0DCvfiN9U5TkKua0VjH9zdxbMOJquhQFVYiGM!/r/dFQBAAAAAAAA";
		case 194: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/iqMnHzq2iIfjnHs.gjY5pMz8HsW6Qpvwo4waMW42q0M!/r/dEcBAAAAAAAA";
		case 195: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/HSdlhO*Her0*AVruJKsV2eWxN7kz*1Ej5Pwf6a.hzt8!/r/dDIBAAAAAAAA";
		case 196: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/CNFQJ06cfQYAhhJUoKxbxRx4XxjFeqB5rit0wFqvyHg!/r/dDEBAAAAAAAA";
		case 197: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/8tqDE9*vbkILqUxhLFpZRky.SZ6zdyyOGL*KF*U1mto!/r/dDYBAAAAAAAA";
		case 198: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/9*5j8fp4xLC8Vvttyn6gMQXKiTh.ukIHzc6x3.*CoyM!/r/dDUBAAAAAAAA";
		case 199: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/EPAoY.Fj.AKNUeI.NG1g0tPxq0yPC17FvgCeN.9HpVI!/r/dPMAAAAAAAAA";
		case 200: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/B4Hd3jelotfBMOT0BZfgKQz8CZufyOv3Haqcc5ILeIY!/r/dD4BAAAAAAAA";
		case 201: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/fVpp1wIKbLASN4vvMWr6pEFm1wX0F6OGaweQ4C0kruc!/r/dFMBAAAAAAAA";
		case 202: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Jb3nMNByGu8QQGsF0KMT2ONjueZ9a24LXYMzaBEfq6c!/r/dFQBAAAAAAAA";
		case 203: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/6Ea3eeLQe.L474O*ce*HOobpiIb2UrJQ5D2wdY9YJ.Q!/r/dFQBAAAAAAAA";
		case 204: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/eC4kh.Rhl9.szlDIZNYIinNVMSuex8bvUYNZkZXOnxs!/r/dDQBAAAAAAAA";
		case 205: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/K1*RDorYI*KhqSwBIVOCQ*7RJRHKgd0*WaStCg30*2k!/r/dFIBAAAAAAAA";
		case 206: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/L0jKCZzf.jnDUQy8qTT1ny1zG6c4kLhwvDR1o*0WzGo!/r/dDYBAAAAAAAA";
		case 207: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/p327fouw1RPOEJZRHxH8e.yxwfSUskFWBrdq3.i4jeE!/r/dFMBAAAAAAAA";
		case 208: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/91VhngsOSbeGCQKaHjBZy1MwFs9RwHf2mkzISUvqYtU!/r/dAgBAAAAAAAA";
		case 209: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/JPS7FkJZNPeB5p5xe6FcLf08.0UPtbllDyAxYNC2gDM!/r/dDQBAAAAAAAA";
		case 210: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/9ZwaKq5txxbSJnlSSHdQGfqeDxT7lmhxDq9NJOG.vuc!/r/dDABAAAAAAAA";
		case 211: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/ge53CxTWhWmu*KmWV8uSpKpGZ3iOJ1pFtydUJm0rmFY!/r/dFIBAAAAAAAA";
		case 212: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/2Lu4qMmXNroWoXxo5*G2jzwy0z6WvWjDlbh4rGHw4qs!/r/dFMBAAAAAAAA";
		case 213: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/mPighB6FLPg670BS99YaGnZ*XccvAHx6Leunun7m5BY!/r/dFQBAAAAAAAA";
		case 214: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/UB4WPpOR9m3WmXG14uxNgP0TcFsCUEHWbmfY3BYQWUA!/r/dDMBAAAAAAAA";
		case 215: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/gvCX1Xak5sMPhasvVRzbo7ZhuiDBz4tuPL5lml4QQiw!/r/dDIBAAAAAAAA";
		case 216: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/vMDinzMFtTlQTaQ1dVTu7zhuK1QtGAKMrel7KzCe5SM!/r/dDEBAAAAAAAA";
		case 217: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/EwB6RJdmei3w.91Gq5VOYWGK3*oHfOoH*T*01h6oz4k!/r/dFMBAAAAAAAA";
		case 218: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/43NnTrdfxguqzCs4I9TEOL3NgioYb4MYDManEkQGRXw!/r/dDQBAAAAAAAA";
		case 219: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Xk5.a2WWcSb2HVh3cI07OjVOGlwoD3642pgALFQqY5I!/r/dDQBAAAAAAAA";
		case 220: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Rc5Rd9IjmZBQNiYUuWZ8nJevlME6FlMrEX2XIU9uEHI!/r/dDUBAAAAAAAA";
		case 221: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/beEZaPHM7f4bbQoKOP0yuuSAeGldQdMfV6ym2FYXucI!/r/dEgBAAAAAAAA";
		case 222: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/FsH5Sa8G2zzfAGMjnhnaFshEGH9ZViJGq.FW6WXFBZs!/r/dDYBAAAAAAAA";
		case 223: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/N7xkQMP9FiJE3LIjTuLBpQxvT14G4gbXojxVKpUjZ28!/r/dFQBAAAAAAAA";
		case 224: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/9fbc.OS1SDnvUCbHFaVx2irBPFQ3TOSn8T51RoHAZ5k!/r/dD4BAAAAAAAA";
		case 225: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/zYSN1eherd1b3IcufS2dEGkrQE3lKA0sEx*IYs6s7VI!/r/dFMBAAAAAAAA";
		case 226: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/tK36zJYuAo38QRYTFbIBKHuegGJ1ksDiC63ODHdwQME!/r/dDEBAAAAAAAA";
		case 227: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/ejm3ueFH8Q3rfQWpNp8ehyKrorFblZYvsqEzLHrCIYw!/r/dFMBAAAAAAAA";
		case 228: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/U5lS1O6l826dMzH6yRb859l8gKfp2*Ipx2qKk8EAc4s!/r/dDUBAAAAAAAA";
		case 229: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/sVX.RSZUzwjka.MsE1vXXk*9*YzPBgkjhAebUw8MBXo!/r/dFMBAAAAAAAA";
		case 230: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/FC0DsPFvFBgzoaYRFI3321V8Ptgr6QLgFzSs0n7md4k!/r/dFQBAAAAAAAA";
		case 231: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/uM6jbM4fGQg3HDZNnl4zRkRR.wtqEJpjY2tOZH4J7PM!/r/dDcBAAAAAAAA";
		case 232: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/fR0jiGbdvsdc.DJwScBDVqxizKXQlDFlkheFI6t0FiM!/r/dDYBAAAAAAAA";
		case 233: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/zh79m1bP*d6eAWCOWlh05t0SH.3BQEvzx*2Dqih0ZnI!/r/dFQBAAAAAAAA";
		case 234: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/AwT3NmQBGdG5naWEUnbSuqjb5882WNscmmXBUexW8xk!/r/dFIBAAAAAAAA";
		case 235: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/b4*KVl67gXFUELnHx8nAYh8TXr8kaWbhaWJKnKWt98A!/r/dFIBAAAAAAAA";
		case 236: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/ZLZAK2*3OqakLFzdjdGtT3aDWRckSubklGO*SWKuRsg!/r/dDMBAAAAAAAA";
		case 237: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/kvvWYRkwwlx4tkqz09oY9TGbz.r3kP7RVaGufcDxoqM!/r/dFIBAAAAAAAA";
		case 238: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/KiW.AsBp.jnM2frRiaxRHfV3.P6POS9XMiy1ak9eCEU!/r/dDYBAAAAAAAA";
		case 239: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/NxhIJnuqyLb814F526m4Mr4lAl2oQ1ATaRhnp7Zveew!/r/dFMBAAAAAAAA";
		case 240: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/2WucUaYTatFqNTuZ*lnIWUITRd3zEWk9jUin6kHoI.k!/r/dFMBAAAAAAAA";
		case 241: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/xO9VbHUK.jZxNqK6bnKGLEjNZnCQgmtqgfTQacuC59g!/r/dFMBAAAAAAAA";
		case 242: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/p5UHQfR7O1etNGxmZ*YODaDhN8cfToQ8CBdqTsMIxAQ!/r/dFIBAAAAAAAA";
		case 243: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/BoQpOJTk0SXDOcosFjxnUB.0RC8Ygm*H5jhIYqgjNI4!/r/dDcBAAAAAAAA";
		case 244: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/IVKXM5hnO3Rat0V9PRwHt6*m9CCMqY5k2HnUfH7WY7U!/r/dFMBAAAAAAAA";
		case 245: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/0jIODGJI77mEhK9dZGJYV*9S.ycDeRs4TE8Nz8gnz*8!/r/dDEBAAAAAAAA";
		case 246: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/upu02pQ5eqPT7RUOmUVb9x6wPxhw5V9thdw83LR9muQ!/r/dFIBAAAAAAAA";
		case 247: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/UZg3iEXH7oY75MKY0NphxvVYr9C1qoEeUzVa88AYkIo!/r/dDYBAAAAAAAA";
		case 248: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/MflCG1denwaEb6iVjBOzKfjyGjIIvjRRiHFJ5mLfNOM!/r/dDYBAAAAAAAA";
		case 249: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/hYMJzg6DB08HVnODZRyguQV94b.7HhZ3WRuTmGE2h*c!/r/dFIBAAAAAAAA";
		case 250: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/tvUGf2DDQLZ.lTOqPxMGopmVTsueRCZRy0SM.VjOiC8!/r/dFIBAAAAAAAA";
		case 251: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/xGF3nnCUnpfH2EhpnmBvoDBlfjcGEyp9QtVVi0lZlCc!/r/dDABAAAAAAAA";
		case 252: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/gOFry4OGx4lyiSWLQAa58UPy.wegD8Bp61MsnUtgYdo!/r/dFIBAAAAAAAA";
		case 253: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/7n23.26*Da6q3DQOBuFxi90NoVkkDSiOGRt6oanXl2I!/r/dDMBAAAAAAAA";
		case 255: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/mUzPLAo1qp2YfpKXObDPnDEv*UFNs8w2slMUSW9aidg!/r/dDQBAAAAAAAA";
		case 256: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/c9zw6Ti7k79xelWE*7RzaEU038C0GAvGmiw2Wd2BxPE!/r/dC4BAAAAAAAA";
		case 257: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/OFAJwJIsprDc**yRsDSUrqIBamLvJTjnevWFnl0IRAk!/r/dDUBAAAAAAAA";
		case 258: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/RkYCO9PbwE04BRiob4U7jKF1AiJh4YcaHww*4mVEgCs!/r/dFIBAAAAAAAA";
		case 259: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/*fN*kMLbAcBsIGNgiAggPh4Tzngd5ydIXuGtMXUtzzA!/r/dAgBAAAAAAAA";
		case 260: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/J0stvkmM3DjmahrwalhWbLSDoEpe0tFI*hT7Xhu54s8!/r/dDcBAAAAAAAA";
		case 261: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/0ynKDo8rceNXV5cGC0VUMWk6RHoofpjFpjS4gJ2kBHs!/r/dC8BAAAAAAAA";
		case 262: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/R817Y9GfvcTQl1HgldhZvR0W1xflj9WMDao6mXmsToU!/r/dFIBAAAAAAAA";
		case 263: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/q*ZtS0nzVGr8r4.iY3rjF00p2cF6qZxjKrmxTc5Z.uA!/r/dFUAAAAAAAAA";
		case 264: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/MxUKebptLYW0XkWpq7VrxS*PAZGk3y5*MZpzIKPqH18!/r/dFIBAAAAAAAA";
		case 265: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/XtE.Xcgwg*oQZj*TGnxvDkyiNIyCquUVz7I.E5ONXbM!/r/dDYBAAAAAAAA";
		case 266: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/D7k8owKV51ckSGMiAp5AL52EdOsmJ6*6As1OkQrYyOM!/r/dFQBAAAAAAAA";
		case 267: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/tpj*1YbJOWltxZTfCHEr5gqorOiI.DqFVk5TYj3vg28!/r/dDUBAAAAAAAA";
		case 268: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/KdlKEahyeKZQBDqInO4S2TiVCPJxk17d44Fau*pvt.g!/r/dDUBAAAAAAAA";
		case 269: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/qDZa0EDXhXc*7wRHjKltMdBa.R6eZOVEmMIrzo5Cx5g!/r/dDUBAAAAAAAA";
		case 270: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/.08GoJv8RQpNUgkh.j9IbL2uR9iJv7m415ggdiJcWNA!/r/dPMAAAAAAAAA";
		case 271: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/kYiZKwuUvj7GcFxf3vdF19K50zX0hb12llIWSE*4Jlk!/r/dFQBAAAAAAAA";
		case 272: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/4efhcdG6sREV*wv5KE*x5IWaUcUFu3a8*pnuKbODDG8!/r/dFMBAAAAAAAA";
		case 273: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/wZt1LksMZ.zGEuFM3**CnNSOPn6j4AwhmxkRzHUbLDY!/r/dFMBAAAAAAAA";
		case 274: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/vx*DzrH3WT9iLW31CSB3KjOmHL8WlFF*lHG7T0bcrFs!/r/dDUBAAAAAAAA";
		case 275: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/eUNEoYBY..83YkhN.jGjyq3jTCLio.oabkoKTzYREms!/r/dDYBAAAAAAAA";
		case 276: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/MxMVFHiOxLg55.pnfYa4jcskIiZKioyIpZ8GP56UoTY!/r/dFQBAAAAAAAA";
		case 277: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/SX0xbqRquCAmejS29EMVwCppOudslW1sObKIAGEVxqM!/r/dFQBAAAAAAAA";
		case 278: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/cDZ6peXJnlRV.rcs0JQXsJ8e4gQ1i8xB.1pFAsQy86Q!/r/dFQBAAAAAAAA";
		case 279: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/SWOFoFGk.xrzd03ZIqMbuDeHzTKjXNCLeKvJhy8CrKo!/r/dDEBAAAAAAAA";
		case 280: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/fWc60*IRoUduhXB*y9Fs*AE*dEl5cxk*BC4HnHO9DuU!/r/dDUBAAAAAAAA";
		case 281: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/6E1DCc.xJb*7Jz*octjFBIoO*uFvjiiPigr1G6cjKF8!/r/dEYBAAAAAAAA";
		case 282: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/muvlJoXwSJ2uIIqwF2kxhb.tJUYxb*2HwEJv2EaSgjE!/r/dDUBAAAAAAAA";
		case 283: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/briyh5HBivAhaSqd4X4ZyC9uiH3Rq.*aVWcWVgXGO9k!/r/dDQBAAAAAAAA";
		case 284: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/d*l59w14I6hucGzwNs*sisCLyEWb8LXf7bWSfoElrqc!/r/dDYBAAAAAAAA";
		case 285: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/4zEepL1*LO5KCeCmIAdQ1RFPWI9ux*4TcB0rk.PRkx4!/r/dFIBAAAAAAAA";
		case 286: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/7aHUhN3rwV5eTyXhXV1G9KPOa4zKphotYtErBM1OqaU!/r/dDUBAAAAAAAA";
		case 287: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/qBidrNwu.VDsmHGsi9g6oEPcGBdAlpZmj1K00j*9Kis!/r/dDQBAAAAAAAA";
		case 288: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/mK*ShNueMzBZHuT7Vtz82wct0iu8J2h.nUqQh2TBBSE!/r/dFQBAAAAAAAA";
		case 289: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/e8buMb4hMRCAQdENI*Av.awr2ZxtqMmfxHc*K*l4ASw!/r/dFQBAAAAAAAA";
		case 290: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/M1x8IQiU772unAbpNLVCTshji64g8dt*nTjjDvcY6XM!/r/dD4BAAAAAAAA";
		case 291: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/fgHXV4WIfqO1AgoBXXKRlFxGdyR4xwV9CumeGwAedjA!/r/dFMBAAAAAAAA";
		case 292: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/z7.taWiXHlygPXi2An4UESABC.YgcvhNsWnBguYn3X4!/r/dDYBAAAAAAAA";
		case 293: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/55YYxy9FxP.xQQqwdNzVLdxynk2ldu.b*2xCArUxioE!/r/dDQBAAAAAAAA";
		case 294: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/d0fIh8JAZ*wmONi8a.JSBWVi*fwLcsy7b1VIO034ALg!/r/dEcBAAAAAAAA";
		case 295: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/3NzZYwVKXTqhFB.gTOUDX5Mkh1u5mmMiwpeMnM8*b30!/r/dFMBAAAAAAAA";
		case 296: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/7PqWU7uVpWPXYyMlKAcQqFil6uYPnHisQXyX6.6SZwA!/r/dDYBAAAAAAAA";
		case 297: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/hy13R8rq4HqfRrXsZrqwtqaO2xs9OTpL4xShjb*q6M8!/r/dD4BAAAAAAAA";
		case 298: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/B9Jq946JSh5qAHuKxxmPxwLcvZlkc0Uhw2Ei9zBUZjc!/r/dDQBAAAAAAAA";
		case 299: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/6J6iQy4nC6xW9tki5csIYxOrzN*PE2T6m3vZ3nHF4hk!/r/dDQBAAAAAAAA";
		case 300: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/faIt2I6Pk.G5fc13mMsS.XNwpL6DgyHigBv*oGG.X8U!/r/dDQBAAAAAAAA";
		case 301: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/czew0cRxbMv09xGXGiWR3t1PDdpudjuejLf2vj.iNRY!/r/dFMBAAAAAAAA";
		case 302: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/ureGqHK30GE7WHbbBO0A*WC4Wf3pjgJm64i3OnXIpx8!/r/dFIBAAAAAAAA";
		case 303: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/gktP4mkbKTB0bl*JCDJgy5y7nNA5jUnmqZ2m8vz6xxM!/r/dDABAAAAAAAA";
		case 304: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/UKtALG5ii0WmNddUcJtCIBl3NNvlu9OPX1VdbOj18mE!/r/dFIBAAAAAAAA";
		case 305: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/FmIoHthCWisLJQ30458y4JD3qEurg.hjSZ1PTs*JKcg!/r/dAgBAAAAAAAA";
		case 306: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/YEU.OMiR6P1ATCflzpJQh40KfpXja.BA*aCFi13UhmI!/r/dDUBAAAAAAAA";
		case 307: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/vHgFqRo9t.xDreJoNPi3I84o633cu*oeTlucDFSUCzA!/r/dFUAAAAAAAAA";
		case 308: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/RTjN5s0DnqSAqBVvxqEFnRJjEGVnDtynPWL5lg9Jt2g!/r/dEcBAAAAAAAA";
		case 309: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/P6O6hf9WSyijmw46*Oxa8FFmoA0prRkqKjANCftw4Mo!/r/dDMBAAAAAAAA";
		case 310: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/wzOQ7KjP.C9sWHG6fvPOcsJxEjcCJbhiK4bo9Ugxb1I!/r/dFIBAAAAAAAA";
		case 311: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/*szkT*Y3wUxxHfl7Ta.BGoGh5xhPKFo0*iDVlkIZHS0!/r/dFMBAAAAAAAA";
		case 312: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/1SvK66DSKEzY.79kuye2pN*j83tHaHpQedIddA6Gu9A!/r/dC0BAAAAAAAA";
		case 313: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/sT0nT*7naqx0vTK8Xx9gR5ZQPOzO8QgUW1OzfOBvuvc!/r/dDYBAAAAAAAA";
		case 314: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/XW6swco0Tcumm9pUY1OYnOdcnOm*cgtEnw2L54KaxJI!/r/dDcBAAAAAAAA";
		case 315: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/rQt8QqPpQbfQeeE*gnLWXHrKvkN60uqcJ1MO.Stuatk!/r/dFMBAAAAAAAA";
		case 316: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/0whUGul3F1FNQefG65PdCGq*nwJ.k3drb.qSC3AgP0o!/r/dDEBAAAAAAAA";
		case 317: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/8j8GzDEunbu3ouBogxrbPhKhh2fc*CBqUimUrPiBNqE!/r/dDYBAAAAAAAA";
		case 318: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/IaDuYOPp3mojEEKsO4zvNQKY0zEgAOBqoDtrPIn14qY!/r/dDUBAAAAAAAA";
		case 319: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/6G65NUe1Yy6ishFzFP6HGlemdQO3Wit7nzVkISMrCIw!/r/dDQBAAAAAAAA";
		case 320: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/u9ozG4RvgMrTpdyuokePGT3QMgtN*NPjrfsCVYgymJ4!/r/dDYBAAAAAAAA";
		case 321: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/hXWptVu0O7jxIktK21hgLTPMPex*hyc3Ysw6HZRfDSM!/r/dDYBAAAAAAAA";
		case 322: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/2lDW1CZ3Bm6*fhQo4VQQJGaNepMcERKJgTkiMVwJ3CU!/r/dDQBAAAAAAAA";
		case 323: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/bSPxlDabA1jCsJ9kNAvqN.BEfN3ELSFNr79lpYomyAI!/r/dDcBAAAAAAAA";
		case 324: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/oniKe9pWHtV3Kuog7nmP1JflDAYNh.5Nbgd4OR6N.kw!/r/dDEBAAAAAAAA";
		case 325: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Vjw6AC0UNLOP8*yyiIfZUej9u72c2MEeVZS03hFwJA8!/r/dDcBAAAAAAAA";
		case 326: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/5rVYwF21Oro9L4h*fOUsym6eUMp.i9AEQJLDjs4Hre4!/r/dFMBAAAAAAAA";
		case 327: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/oxg7l1x39zdps50MfgZCeocfPUxpxYB*qmNheuQKY1M!/r/dDcBAAAAAAAA";
		case 328: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/mHjr5ZOE.XZDgq72KrPuPdaAOXLTjlBnOXxz8We6ulM!/r/dFMBAAAAAAAA";
		case 329: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/B1Qd*1KcsC32r57CQgEo7j9C0f4PkyaU86FvKWZfghU!/r/dFIBAAAAAAAA";
		case 330: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/otOMXj.8ic.DSBNyQyIYQPbkhNCH0soGMLQYs036JW8!/r/dC4BAAAAAAAA";
		case 331: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Y9lDbZUkfGEyI.5dct.TwDdO*5k3NOvNC1v8L1rMJxA!/r/dFIBAAAAAAAA";
		case 332: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/xhZMWqjkiYVNKsz80UUtF3Lz2ySjGu6AvygmJ0W4xg4!/r/dDYBAAAAAAAA";
		case 333: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/TbI9VMhAR*Ao*WebBVTdr37*l7lWNEDunfO4bR0prQo!/r/dDABAAAAAAAA";
		case 334: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/fBm*A*AVnNTEBMFz880TATbr.JRG8UfdusJxPgFeRU8!/r/dC4BAAAAAAAA";
		case 335: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Z408wIptoOOBx*aVdE5cHp4OedtTGHcDhn5Kd99OHgM!/r/dDUBAAAAAAAA";
		case 336: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/szQb*ZB72*y3965SkKL5XbBu8WSsuBDrSP27xo6m6Ag!/r/dDEBAAAAAAAA";
		case 337: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/oSTPvhwkPOILOZrWES7hltdyMos0sRSvrJvoJS6tAHI!/r/dDEBAAAAAAAA";
		case 338: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/i2SeKnFwwrUI9uWcL8CwOT7bmP9JvLArmK3z5mzFr4w!/r/dEcBAAAAAAAA";
		case 339: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/QmsgugI5IuoHaOFdT.ZJfFopEM61enW9wEA*GOU0yoo!/r/dFQBAAAAAAAA";
		case 340: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/32iBxtWpJJdGx45WYkyIS9fhAs0SB5Lm6TgHwluzIN8!/r/dDQBAAAAAAAA";
		case 341: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/9zUj8.MuCS8XziGUvCD1eE0FpzD4CixbQMXa6k7FKTI!/r/dEcBAAAAAAAA";
		case 342: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/NnFDPd3kmtcJ4XxX3EKaTKVq7j6wtEGeKYsI*kkQStM!/r/dDYBAAAAAAAA";
		case 343: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/kH.o5ClxA3BpmjdneP4eIaUfCrNLGqLcO6UpUwN2SMA!/r/dAgBAAAAAAAA";
		case 344: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/uFnvF.RBm*MPa7*yX8IrfRwZphGMJuGkXWXD9qXUimI!/r/dPMAAAAAAAAA";
		case 345: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/8dfei5ZMt8r3z0D4kX5.dbTzQLTJbScaT*uSifXR4Bw!/r/dFMBAAAAAAAA";
		case 346: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/fdLWdkcyj.by9DHQUvvy4S1g1eD.R.HbDwkUiVrZyUE!/r/dFYBAAAAAAAA";
		case 347: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/lvSaKWAzJDM5c51IcrSDxELrVv080khw6XlLq4yAXWA!/r/dDcBAAAAAAAA";
		case 348: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Et267RXXu.q*o98dnV9FHUNfr02t3eKtp.VEVvsAvE8!/r/dFMBAAAAAAAA";
		case 349: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/TWcjd6b9jVUeFB4wuoToGp.qE6Wla5rUqztDxDtIpnc!/r/dFQBAAAAAAAA";
		case 350: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/*vVn3bhJ*l8k1wFJ*IDOiT*26M304Ikd9FQn*mXD9dk!/r/dD4BAAAAAAAA";
		case 351: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/zlR642O3I3XFzL21vUGHToEzUlube5P*Ak7KCsRkUpw!/r/dFQBAAAAAAAA";
		case 352: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/tGQSTyecry*jJem*5Tz6M4j5TEe5pek6bjDZSVeapeU!/r/dDcBAAAAAAAA";
		case 353: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/I6gd3WSk5viBuab*hvmePrhHAd0Du1X5j5K9vPh*Dh4!/r/dFIBAAAAAAAA";
		case 354: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/uoRInTActefusvyImYlJj9kW*uOcnvcBz1RWIOt4h7o!/r/dFIBAAAAAAAA";
		case 355: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/jK7susfHHt3swvBND.h*BRz.9wAd1h9lAM63r1xHw*A!/r/dC0BAAAAAAAA";
		case 356: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/xLXZw6*HbuE5npaIlAIZoLr0rSwZUzKsqDFrmTM13LI!/r/dFYBAAAAAAAA";
		case 357: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Q36Gc0vRT7eGiC582Ioma40*KshdESjJbAf7y7dKg3o!/r/dFIBAAAAAAAA";
		case 358: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/aBHIg7lD33C7xGFJkvOoNlZOysFpEu13EvMqCyPr6QI!/r/dEgBAAAAAAAA";
		case 359: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/0KF*EsEz*rWFC6SrntwOUQOVCRw6wOIrwuk8M3g7ZpE!/r/dDcBAAAAAAAA";
		case 360: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/TKEZGwK52HswJG96HTCwgGGS*HP3MGH0BqUSJkp2hZ8!/r/dEgBAAAAAAAA";
		case 361: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Fy9kdT1wEHFUWNu0ZxXu.YqJKowziJOCZ.IgKenG9f8!/r/dFQBAAAAAAAA";
		case 362: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Lpf.Fwu3lQw4AYCDMS67opntfMwBb5DNZqXif4eTabE!/r/dDYBAAAAAAAA";
		case 363: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/ZfHoFct*2qF8BwdB*8WgnyGVXhXPuGk*UkDe7u*Jd1g!/r/dDYBAAAAAAAA";
		case 364: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/iYhSlyNLyLMjSmgj1egkOGjiw5uAs26Rq06cfkrS3ck!/r/dFMBAAAAAAAA";
		case 365: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/4CXpUtQJ7XPKUUvO5s27**9b6Gl*oV4cAC.QypHexTU!/r/dDYBAAAAAAAA";
		case 366: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/hUCL*1CwEojqHLsxKYnb22.26aA9YdvPdJHNE*9EbAM!/r/dDUBAAAAAAAA";
		case 367: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/VugGVzl4qdCiahXtuSgYb6bqMIoArBgY*4N0MAw5EmY!/r/dFQBAAAAAAAA";
		case 368: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/MysJpYqrcwPPHJ7jzCWFdz*5fEKrue0rtzGXL9eldks!/r/dFIBAAAAAAAA";
		case 369: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/IXBVF3dJ191yDxpgjU1jcOlIkPel5FokgZmVR3VznUs!/r/dD4BAAAAAAAA";
		case 370: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/BpY3J2Qz0BrDc7C.kA1sfdaL6wqSYbFuMprBk2J9xOQ!/r/dDABAAAAAAAA";
		case 371: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/2gyMPpZKS5dsoL39GXtViShh5VnXGLzP3CXbZbxRTGA!/r/dDABAAAAAAAA";
		case 372: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/VzuU*MDpBEpDAn*F.U1*Z83Mq4*h1coYP6qjoxaTloo!/r/dD4BAAAAAAAA";
		case 373: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/4Z5j5mbzI7LdkNY4RAgXdOGEKBe3vVrM0.qKSqtBYgA!/r/dFQBAAAAAAAA";
		case 374: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/t4oSXV31PfTjDkJmUd3pXKgY1Hd3Hz8ZenRUKNYZpfA!/r/dDIBAAAAAAAA";
		case 375: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/fe6Tt0*mXEm690huPONx*VpbtxDCnQNjYIgmqLRAQQc!/r/dFIBAAAAAAAA";
		case 376: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/9Txi8Gg0dgo.OZwEkSUh*w4p3JGQVLMOnEsMM3JZPxI!/r/dFIBAAAAAAAA";
		case 377: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/t6QJwoJtYQoITf51ktiuhTGWT0DaUh8UNggwoNNcoIo!/r/dDcBAAAAAAAA";
		case 378: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/HKlfirRK8m1REnW0myO9aupx8tv1qKLHfs.i88nYZXk!/r/dDYBAAAAAAAA";
		case 379: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/lV2dxnhpQHr9EiCA9uYHmokNOqw6KpDNWVIpEheIRck!/r/dFIBAAAAAAAA";
		case 380: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/2Y6dMrImbZrQZ8qpAd5QS5Ws2D6Dcp4j0ye1CfjFsR0!/r/dFQBAAAAAAAA";
		case 381: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/rcSWRWXng0lEjN8gXZH88xzlCHpYlcVI.01p0LMEQaE!/r/dFQBAAAAAAAA";
		case 382: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/Y6rZ5JPPc8X9e75MS0U42cVTe4ag7uQFLAjHPxkd*bA!/r/dDQBAAAAAAAA";
		case 383: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/PeG6c7oFR*PiSK2jd1M5MLE88NoJCSrBbiQ7ojhTKX8!/r/dFMBAAAAAAAA";
		case 384: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/1krt7fzgqAgBNnwczTnZ.wZdkhhzgz9.hPcH.tobwtw!/r/dDQBAAAAAAAA";
		case 385: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/5ARLLAYH9M0efj0pcBH.Z0ZGK0tUEBu18o5H1jA3Jh0!/r/dFMBAAAAAAAA";
		case 386: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/rS3amsolyTm0Lf0OsnInS89p7lk33omTsq9yrqqH9Jc!/r/dFQBAAAAAAAA";
		case 387: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/pRJckmLX1gOFoxulNeXtTkXmsI73tb*lRlR9K29AWlg!/r/dDQBAAAAAAAA";
		case 388: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/1HLMlruzmG3Ov3TKEhZ6C0XR1ACfIZ.WRiw4BjYDbes!/r/dAgBAAAAAAAA";
		case 389: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/.AJkqfc03lXAWSd0pz1yCXPtB7CGfye2876Yb6MegKA!/r/dEkBAAAAAAAA";
		case 390: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/dC9TgmGzf3UzpHxETW3fxwWB.RAa0cLN1xoXceqACd8!/r/dDUBAAAAAAAA";
		case 391: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/wf0NG4F70orU9Gtsgcam8r*WmqwajCjykpetTvaEJyA!/r/dDUBAAAAAAAA";
		case 392: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/jjKi*jHrRQW4YEWT8qvcLuK5cDB4QMsSDCUAfL5P0Ak!/r/dDMBAAAAAAAA";
		case 393: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/IQhnB36QhbXcxJpn35axBVjmhynbiLOszc8UzTuJERo!/r/dFMBAAAAAAAA";
		case 394: return "http://r.photo.store.qq.com/psb?/V12iQzsN1HppyH/0XA2k*T1zv4Xx*E5hCheYmr349F3VjzXur1nPggEPs0!/r/dEkBAAAAAAAA";
	}
}

string make_card(int num, int ID) //[CQ:share,url={1},title={2},content={3},image={4}]
{
	ostringstream ss;
	ss << "[CQ:share,url=" << URL(ID) << ",title=" << num << ",content=点击查看大图,image=" << URL(ID) << "]";
	return ss.str();
}

void AT(int64_t Group,int64_t QQ,string msg)
{
	ostringstream ss;
	ss << "[CQ:at,qq=" << QQ << "] "<< msg;
	string s_tmp = ss.str();
	CQ_sendGroupMsg(ac, Group, s_tmp.c_str());
}

int get_rand(int num)
{
	static mt19937 generator{ random_device()() };
	uniform_int_distribution<>distribution(0, num - 1);
	return distribution(generator);
}

void show_card(int64_t QQ)
{
	/*ostringstream ss;
	ss << "您的当前的手牌是：\n";
	for (int i = 0; i < 6; i++)
	{
		ss << "[CQ:image,file=cards (" << player_card[QQ][i] << ").jpg]\n";
	}
	ss << "自上而下编号为1~6";
	string s_tmp = ss.str();
	CQ_sendPrivateMsg(ac,QQ,s_tmp.c_str());*/
	CQ_sendPrivateMsg(ac, QQ, "您当前的手牌是");
	for (int i = 0; i < 6; i++)
	{
		CQ_sendPrivateMsg(ac, QQ, make_card(i+1,player_card[QQ][i]).c_str());
	}
}

void show_vote(int64_t Group)
{
	ostringstream ss;
	ss << "投票结果：\n";
	for (int i = 0; i < player_cnt[Group]; i++)
	{
		if (player_quit[game_player[Group][i]] && i!=game_round[Group]-1) continue;
		ss << char('A'+i) <<"("<< card_num[Group][card_sel[game_player[Group][i]]]+1 << ")：";
		for (int j = 0; j < get_vote[game_player[Group][i]].size(); j++)
		{
			ss << char('A'+get_vote[game_player[Group][i]][j]-1) << " ";
		}
		ss << "\n";
	}
	ss << "本轮的正确谜底是：";
	string s_tmp = ss.str();
	CQ_sendGroupMsg(ac, Group, s_tmp.c_str());
	CQ_sendGroupMsg(ac, Group, make_card(card_num[Group][card_sel[game_player[Group][game_round[Group] - 1]]] + 1, card_sel[game_player[Group][game_round[Group] - 1]]).c_str());
}

void deliver_card(int64_t QQ)
{
	while (player_card[QQ].size() < 6)
	{
		int rnd = get_rand(card_cnt) + 1;
		if (!cards[in_game[QQ]][rnd])
		{
			player_card[QQ].push_back(rnd);
			cards[in_game[QQ]][rnd] = true;
		}
	}
	//show_card(QQ);
}

void change_card(int64_t Group, int64_t QQ,int num)
{
	int rnd;
	do
	{
		rnd = get_rand(card_cnt) + 1;
	} while (cards[Group][rnd]);
	cards[Group][rnd] = true;
	player_card[QQ][num] = rnd;
}

void end_game(int64_t Group)
{
	if (!game_status[Group]) return;
	game_status[Group] = 0;
	int maxs = 0;
	CQ_sendGroupMsg(ac, Group, "游戏结束");
	time_t rawtime;
	time(&rawtime);
	tm *now_time = localtime(&rawtime);
	//CQ_sendGroupMsg(ac, Group, (date_d(now_time) + " " + date_d(pre_upd)).c_str());
	if (date_d(now_time).compare(date_d(pre_upd)))
	{
		score_d.clear();
		win_d.clear();
	}
	if (date_w(now_time).compare(date_w(pre_upd)))
	{
		score_w.clear();
		win_w.clear();
	}
	if (date_m(now_time).compare(date_m(pre_upd)))
	{
		score_m.clear();
		win_m.clear();
	}
	pre_upd = now_time;
	for (int i = 0; i < player_cnt[Group]; i++)
	{
		if (player_quit[game_player[Group][i]]) continue;
		in_game[game_player[Group][i]] = 0;
		CQ_sendPrivateMsg(ac, game_player[Group][i], "游戏结束");
		if (player_score[game_player[Group][i]] > maxs) maxs = player_score[game_player[Group][i]];
		score_d[game_player[Group][i]] += player_score[game_player[Group][i]];
		score_w[game_player[Group][i]] += player_score[game_player[Group][i]];
		score_m[game_player[Group][i]] += player_score[game_player[Group][i]];
	}
	for (int i = 0; i < player_cnt[Group]; i++)
	{
		if (player_quit[game_player[Group][i]]) continue;
		if (player_score[game_player[Group][i]] == maxs)
		{
			AT(Group, game_player[Group][i], "获胜！");
			win_d[game_player[Group][i]]++;
			win_w[game_player[Group][i]]++;
			win_m[game_player[Group][i]]++;
		}
	}
	game_player[Group].clear();
	ostringstream ss;
	ss << "C:\\Users\\Administrator\\Desktop\\CQP\\data\\ingame\\" << Group;
	string s_tmp = ss.str();
	remove(s_tmp.c_str());
	write_score();
}

void quit_game(int64_t QQ);

void start_rid(int64_t Group)
{
	if (!game_status[Group] || game_status[Group] == RID) return;
	while (player_quit[game_player[Group][game_round[Group] - 1]] && game_round[Group]<=player_cnt[Group])
	{
		game_round[Group]++;
	}
	if (game_round[Group] > player_cnt[Group])
	{
		end_game(Group);
		return;
	}
	for (int i = 0; i < player_cnt[Group]; i++)
	{
		if (player_quit[game_player[Group][i]]) continue;
		show_card(game_player[Group][i]);
	}
	//CQ_sendGroupMsg(ac,Group,"请各位查看私信");
	game_status[Group] = RID;
	int round_now = game_round[Group];
	//player_status[game_player[Group][Round - 1]] = RID;
	ostringstream ss;
	ss << "本轮由" << char('A'+game_round[Group]-1)  << ". [CQ:at,qq="<< game_player[Group][game_round[Group] - 1] << "] 出题，请注意查看私信";
	string s_tmp = ss.str();
	CQ_sendGroupMsg(ac,Group,s_tmp.c_str());
	for (int i = 0; i < player_cnt[Group]; i++)
	{
		if (player_quit[game_player[Group][i]]) continue;
		if (i == game_round[Group] - 1) CQ_sendPrivateMsg(ac, game_player[Group][i], "请给出您的描述，限时2分钟");
		else CQ_sendPrivateMsg(ac, game_player[Group][i], "请等待出题");
	}
	DELAY(120);
	if (player_quit[game_player[Group][game_round[Group] - 1]]) return;
	if (game_round[Group] == round_now && game_status[Group] == RID)
	{
		//in_game[game_player[Group][game_round[Group] - 1]] = 0;
		AT(Group, game_player[Group][game_round[Group] - 1], "超时");
		CQ_sendPrivateMsg(ac, game_player[Group][game_round[Group] - 1], "超时");
		quit_game(game_player[Group][game_round[Group] - 1]);
	}
}

void start_sel(int64_t Group)
{
	if (!game_status[Group] || game_status[Group] == SEL) return;
	game_status[Group] = SEL;
	int round_now = game_round[Group];
	tmp_cnt[Group] = 0;
	ostringstream ss;
	ss << "本轮的谜面是：" << game_rid[Group] << "\n请各位私聊选择手牌";
	string s_tmp = ss.str();
	CQ_sendGroupMsg(ac, Group, s_tmp.c_str());
	ss.str("");
	ss << "本轮的谜面是：" << game_rid[Group] << "\n请回复1~6选择您的要出的牌，限时2分钟";
	s_tmp = ss.str();
	for (int i = 0; i < player_cnt[Group]; i++)
	{
		if (player_quit[game_player[Group][i]]) continue;
		player_done[game_player[Group][i]] = false;
		CQ_sendPrivateMsg(ac, game_player[Group][i], s_tmp.c_str());
	}
	DELAY(120);
	for (int i = 0; i < player_cnt[Group]; i++)
	{
		if (game_status[Group] != SEL || game_round[Group] != round_now) break;
		if (player_quit[game_player[Group][i]]) continue;
		if (!player_done[game_player[Group][i]])
		{
			//in_game[game_player[Group][i]] = 0;
			AT(Group, game_player[Group][i], "超时");
			CQ_sendPrivateMsg(ac, game_player[Group][i], "超时");
			quit_game(game_player[Group][i]);
		}
	}
}

void start_vote(int64_t Group)
{
	if (!game_status[Group] || game_status[Group] == VOTE) return;
	game_status[Group] = VOTE;
	int round_now = game_round[Group];
	tmp_cnt[Group] = 0;
	CQ_sendGroupMsg(ac, Group, "思考时间到，请各位私聊进行投票");
	for (int i = 0; i < player_cnt[Group]; i++)
	{
		if (player_quit[game_player[Group][i]]) continue;
		player_done[game_player[Group][i]] = false;
		ans_right[game_player[Group][i]] = false;
		get_vote[game_player[Group][i]].clear();
		if (i==game_round[Group]-1) CQ_sendPrivateMsg(ac, game_player[Group][i], "请等待投票完成");
		else CQ_sendPrivateMsg(ac, game_player[Group][i],"请回复图片编号进行投票，限时2分钟");
	}
	DELAY(120);
	for (int i = 0; i < player_cnt[Group]; i++)
	{
		if (game_status[Group] != VOTE || game_round[Group] != round_now) break;
		if (player_quit[game_player[Group][i]]) continue;
		if (i!=game_round[Group]-1 && !player_done[game_player[Group][i]])
		{
			//in_game[game_player[Group][i]] = 0;
			AT(Group, game_player[Group][i], "超时");
			CQ_sendPrivateMsg(ac, game_player[Group][i], "超时");
			quit_game(game_player[Group][i]);
		}
	}
}

void start_talk(int64_t Group)
{
	if (!game_status[Group] || game_status[Group] == TALK) return;
	game_status[Group] = TALK;
	card_tmp[Group].clear();
	for (int i = 0; i < player_cnt[Group]; i++)
	{
		if (player_quit[game_player[Group][i]]) continue;
		card_tmp[Group].push_back(card_sel[game_player[Group][i]]);
		CQ_sendPrivateMsg(ac, game_player[Group][i], "所有玩家选择完毕，请前往群聊查看");
	}
	srand(unsigned(time(0)));
	random_shuffle(card_tmp[Group].begin(), card_tmp[Group].end());
	ostringstream ss;
	CQ_sendGroupMsg(ac, Group, "本轮候选牌是：");
	for (int i = 0; i < card_tmp[Group].size(); i++)
	{
		card_ori[Group][i] = card_tmp[Group][i];
		card_num[Group][card_tmp[Group][i]] = i;
		//ss << "[CQ:image,file=cards (" << card_tmp[Group][i] << ").jpg]\n";
		CQ_sendGroupMsg(ac, Group, make_card(i+1,card_tmp[Group][i]).c_str());
	}
	//ss << "自上而下编号为1~" << player_cnt[Group]-quit_cnt[Group];
	//string s_tmp = ss.str();
	//CQ_sendPrivateMsg(ac, QQ, s_tmp.c_str());
	//CQ_sendGroupMsg(ac, Group, s_tmp.c_str());
	CQ_sendGroupMsg(ac, Group, "现在是30秒查看与思考时间");
	DELAY(30);
	start_vote(Group);
}

void end_round(int64_t Group)
{
	if (!game_status[Group] || game_status[Group] == GAP) return;
	game_status[Group] = GAP;
	show_vote(Group);
	DELAY(5);
	ostringstream ss;
	ss << "得分情况：\n";
	for (int i = 0; i < player_cnt[Group]; i++)
	{
		if (player_quit[game_player[Group][i]]) continue;
		CQ_sendPrivateMsg(ac, game_player[Group][i], "所有玩家投票完毕，请前往群聊查看");
		ss << char('A'+i) << "：";
		if (i == game_round[Group] - 1)
		{
			if (get_vote[game_player[Group][i]].size() == 0 || get_vote[game_player[Group][i]].size() == player_cnt[Group] - quit_cnt[Group] - 1)
			{
				ss << "+0 " << player_score[game_player[Group][i]];
			}
			else
			{
				player_score[game_player[Group][i]] += 3;
				ss << "+3 " << player_score[game_player[Group][i]];
			}
		}
		else
		{
			if (get_vote[game_player[Group][game_round[Group]-1]].size() == 0 || get_vote[game_player[Group][game_round[Group] - 1]].size() == player_cnt[Group] - quit_cnt[Group] - 1)
			{
				player_score[game_player[Group][i]] += 2 + get_vote[game_player[Group][i]].size();
				ss << "+" << 2 + get_vote[game_player[Group][i]].size() << " " << player_score[game_player[Group][i]];
			}
			else
			{
				player_score[game_player[Group][i]] += (ans_right[game_player[Group][i]]?3:0) + get_vote[game_player[Group][i]].size();
				ss << "+" << (ans_right[game_player[Group][i]] ? 3 : 0) + get_vote[game_player[Group][i]].size() << " " << player_score[game_player[Group][i]];
			}
		}
		if (i != player_cnt[Group] - 1) ss << "\n";
	}
	string s_tmp = ss.str();
	CQ_sendGroupMsg(ac, Group, s_tmp.c_str());
	DELAY(5);
	game_round[Group]++;
	start_rid(Group);
}

void game_start(int64_t Group)
{
	if (!game_status[Group] || game_round[Group] || game_status[Group] == GAP) return;
	game_status[Group] = GAP;
	game_round[Group] = 1;
	pre_sign[Group] = 0;
	ostringstream ss;
	ss << "游戏开始，本次游戏玩家如下：\n";
	for (int i = 0; i < player_cnt[Group]; i++)
	{
		ss << char('A'+i) << ". [CQ:at,qq=" << game_player[Group][i] << "] " << "\n";
		player_num[game_player[Group][i]] = i + 1;
	}
	ss << "请注意查收私信，私信回复“help”可查看帮助";
	string s_tmp = ss.str();
	CQ_sendGroupMsg(ac, Group, s_tmp.c_str());
	DELAY(5);
	//player_cnt[Group] = 0;
	//quit_cnt[Group] = 0;
	//game_player[Group].clear();
	cards[Group].clear();
	cards[Group][79] = true;
	cards[Group][254] = true;
	game_rid[Group].clear();
	card_ori[Group].clear();
	card_num[Group].clear();
	for (int i = player_cnt[Group]; i < game_player[Group].size(); i++)
	{
		in_game[game_player[Group][i]] = 0;
	}
	game_player[Group].erase(game_player[Group].begin() + player_cnt[Group], game_player[Group].end());
	for (int i = 0; i < player_cnt[Group]; i++)
	{
		if (player_quit[game_player[Group][i]]) continue;
		player_card[game_player[Group][i]].clear();
		player_score[game_player[Group][i]] = 0;
		//player_quit[game_player[Group][i]] = false;
		deliver_card(game_player[Group][i]);
		//DELAY(1);
	}
	//game_round[Group] = 1;
	start_rid(Group);
}

void quit_game(int64_t QQ)
{
	int64_t Group = in_game[QQ];
	in_game[QQ] = 0;
	player_quit[QQ] = true;
	quit_cnt[Group]++;
	CQ_sendPrivateMsg(ac,QQ,"您已退出游戏");
	AT(Group, QQ, "退出游戏");
	if (player_cnt[Group] - quit_cnt[Group] < 4)
	{
		game_status[Group] = 0;
		for (int i = 0; i < player_cnt[Group]; i++)
		{
			if (player_quit[game_player[Group][i]]) continue;
			in_game[game_player[Group][i]] = 0;
			CQ_sendPrivateMsg(ac, game_player[Group][i], "游戏结束");
		}
		game_player[Group].clear();
		CQ_sendGroupMsg(ac, Group, "人数不足，游戏终止");
		ostringstream ss;
		ss << "C:\\Users\\Administrator\\Desktop\\CQP\\data\\ingame\\" << Group;
		string s_tmp = ss.str();
		remove(s_tmp.c_str());
		return;
	}
	if (game_status[Group] == RID && player_num[QQ] == game_round[Group])
	{
		game_status[Group] = GAP;
		game_round[Group]++;
		start_rid(Group);
		return;
	}
	if (game_status[Group] == SEL && tmp_cnt[Group] == player_cnt[Group] - quit_cnt[Group])
	{
		if (player_num[QQ] == game_round[Group])
		{
			game_round[Group]++;
			start_rid(Group);
			return;
		}
		start_talk(Group);
		return;
	}
	if (game_status[Group] == VOTE && tmp_cnt[Group] == player_cnt[Group] - quit_cnt[Group] - 1)
	{
		end_round(Group);
		return;
	}
}

/*
* Type=21 私聊消息
* subType 子类型，11/来自好友 1/来自在线状态 2/来自群 3/来自讨论组
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t msgId, int64_t fromQQ, const char *msg, int32_t font) {
	if (!strcmp(msg, "timetst"))
	{
		time_t rawtime;
		time(&rawtime);
		tm *now_time = localtime(&rawtime);
		CQ_sendPrivateMsg(ac, fromQQ, (date_d(now_time) + " " + date_d(pre_upd)).c_str());
		return EVENT_IGNORE;
	}
	if (!strcmp(msg, "cardtst"))
	{
		for (int i = 1; i <= 6; i++)
		{
			CQ_sendPrivateMsg(ac, fromQQ, make_card(i,i).c_str());
		}
		return EVENT_IGNORE;
	}
	if (!strcmp(msg, "查询dixit"))
	{
		ostringstream ss;
		ss << "您的今日得分：" << score_d[fromQQ] << "\n"
			<< "您的本周得分：" << score_w[fromQQ] << "\n"
			<< "您的本月得分：" << score_m[fromQQ] << "\n"
			<< "您的今日获胜数：" << win_d[fromQQ] << "\n"
			<< "您的本周获胜数：" << win_w[fromQQ] << "\n"
			<< "您的本月获胜数：" << win_m[fromQQ];
		string s_tmp = ss.str();
		CQ_sendPrivateMsg(ac, fromQQ, s_tmp.c_str());
		return EVENT_IGNORE;
	}
	if (in_game[fromQQ])
	{
		int64_t fromGroup = in_game[fromQQ];
		if (!strcmp(msg, "help"))
		{
			CQ_sendPrivateMsg(ac, fromQQ, "游戏规则请自行百度dixit或只言片语，建议查看dixit维基百科\n目前支持的私聊指令：\nhelp：帮助\ncard：查看当前手牌\nnum：查询玩家编号\n退出：退出游戏");
			return EVENT_IGNORE;
		}
		if (!strcmp(msg, "card"))
		{
			show_card(fromQQ);
			return EVENT_IGNORE;
		}
		if (!strcmp(msg, "num"))
		{
			ostringstream ss;
			ss << "您的编号是：" << char('A'+player_num[fromQQ]-1);
			string s_tmp = ss.str();
			CQ_sendPrivateMsg(ac, fromQQ, s_tmp.c_str());
			return EVENT_IGNORE;
		}
		if (!strcmp(msg, "退出"))
		{
			if (game_status[fromGroup] == SIGN)
			{
				if (fromQQ == game_player[fromGroup][0])
				{
					game_status[fromGroup] = 0;
					pre_sign[fromGroup] = 0;
					for (int i = 0; i < game_player[fromGroup].size(); i++)
					{
						in_game[game_player[fromGroup][i]] = 0;
					}
					game_player[fromGroup].clear();
					CQ_sendGroupMsg(ac, fromGroup, "发起人退出，报名终止");
					ostringstream ss;
					ss << "C:\\Users\\Administrator\\Desktop\\CQP\\data\\ingame\\" << fromGroup;
					string s_tmp = ss.str();
					remove(s_tmp.c_str());
				}
				else
				{
					in_game[fromQQ] = 0;
					game_player[fromGroup].erase(find(game_player[fromGroup].begin(), game_player[fromGroup].end(), fromQQ));
					player_cnt[fromGroup]--;
					AT(fromGroup, fromQQ, "取消报名");
				}
			}
			else
			{
				quit_game(fromQQ);
			}
			return EVENT_IGNORE;
		}
		if (game_status[fromGroup] == RID && fromQQ == game_player[fromGroup][game_round[fromGroup]-1])
		{
			game_rid[fromGroup] = msg;
			start_sel(fromGroup);
			return EVENT_IGNORE;
		}
		if (game_status[fromGroup] == SEL && !player_done[fromQQ] && strlen(msg) == 1 && msg[0] >= '1' && msg[0] <= '6')
		{
			player_done[fromQQ] = true;
			card_sel[fromQQ] = player_card[fromQQ][msg[0] - '1'];
			card_from[fromGroup][player_card[fromQQ][msg[0] - '1']] = fromQQ;
			tmp_cnt[fromGroup]++;
			change_card(fromGroup,fromQQ, msg[0] - '1');
			CQ_sendPrivateMsg(ac, fromQQ, "选择成功");
			if (tmp_cnt[fromGroup] == player_cnt[fromGroup]-quit_cnt[fromGroup])
			{
				start_talk(fromGroup);
			}
			return EVENT_IGNORE;
		}
		if (game_status[fromGroup] == VOTE && !player_done[fromQQ] && fromQQ!=game_player[fromGroup][game_round[fromGroup]-1] && strlen(msg) == 1 && msg[0] >= '1' && msg[0] <= '0' + player_cnt[fromGroup])
		{
			if (card_from[fromGroup][card_ori[fromGroup][msg[0] - '1']] == fromQQ)
			{
				CQ_sendPrivateMsg(ac, fromQQ, "不能投自己");
				return EVENT_IGNORE;
			}
			player_done[fromQQ] = true;
			if (card_from[fromGroup][card_ori[fromGroup][msg[0] - '1']] == game_player[fromGroup][game_round[fromGroup] - 1]) ans_right[fromQQ] = true;
			get_vote[card_from[fromGroup][card_ori[fromGroup][msg[0] - '1']]].push_back(player_num[fromQQ]);
			CQ_sendPrivateMsg(ac, fromQQ, "投票成功");
			tmp_cnt[fromGroup]++;
			if (tmp_cnt[fromGroup] == player_cnt[fromGroup]-quit_cnt[fromGroup]-1)
			{
				end_round(fromGroup);
			}
			return EVENT_IGNORE;
		}
		return EVENT_IGNORE;
	}
	//如果要回复消息，请调用酷Q方法发送，并且这里 return EVENT_BLOCK - 截断本条消息，不再继续处理  注意：应用优先级设置为"最高"(10000)时，不得使用本返回值
	//如果不回复消息，交由之后的应用/过滤器处理，这里 return EVENT_IGNORE - 忽略本条消息
	return EVENT_IGNORE;
}


/*
* Type=2 群消息
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t msgId, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	/*(if (!strcmp(msg, "[CQ:at,qq=3513312871] 猜歌") && !game_status[fromGroup])
	{
		other_game[fromGroup] = true;
		return EVENT_IGNORE;
	}
	if (fromQQ == 3513312871 && !strcmp(msg, "游戏结束"))
	{
		other_game[fromGroup] = false;
		return EVENT_IGNORE;
	}*/
	if (!strcmp(msg, "[CQ:at,qq=3513312871] dixit日榜"))
	{
		show_score(1, fromGroup);
		return EVENT_IGNORE;
	}
	if (!strcmp(msg, "[CQ:at,qq=3513312871] dixit周榜"))
	{
		show_score(2, fromGroup);
		return EVENT_IGNORE;
	}
	if (!strcmp(msg, "[CQ:at,qq=3513312871] dixit月榜"))
	{
		show_score(3, fromGroup);
		return EVENT_IGNORE;
	}
	if (!strcmp(msg, "[CQ:at,qq=3513312871] 只言片语") || !strcmp(msg, "[CQ:at,qq=3513312871] dixit"))
	{
		if (in_game[fromQQ])
		{
			AT(fromGroup, fromQQ, "您已加入一场游戏");
			return EVENT_IGNORE;
		}
		if (game_status[fromGroup])
		{
			AT(fromGroup, fromQQ, "当前群正在游戏中");
			return EVENT_IGNORE;
		}
		ostringstream ss;
		ss << "C:\\Users\\Administrator\\Desktop\\CQP\\data\\ingame\\" << fromGroup;
		ifstream ifs(ss.str());
		if (ifs)
		{
			AT(fromGroup, fromQQ, "当前群正在进行其他游戏");
			ifs.close();
			return EVENT_IGNORE;
		}
		game_status[fromGroup] = SIGN;
		in_game[fromQQ] = fromGroup;
		ofstream ofs(ss.str());
		ofs.close();
		game_player[fromGroup].push_back(fromQQ);
		player_cnt[fromGroup] = 1;
		player_quit[fromQQ] = false;
		quit_cnt[fromGroup] = 0;
		game_round[fromGroup] = 0;
		pre_sign[fromGroup] = time(0);
		time_t tmp_t = pre_sign[fromGroup];
		CQ_sendGroupMsg(ac, fromGroup, "只言片语游戏准备开始，回复“报名”即可加入");
		DELAY(180);
		if (pre_sign[fromGroup] == tmp_t)
		{
			game_status[fromGroup] = 0;
			pre_sign[fromGroup] = 0;
			for (int i = 0; i < game_player[fromGroup].size(); i++)
			{
				in_game[game_player[fromGroup][i]] = 0;
			}
			game_player[fromGroup].clear();
			CQ_sendGroupMsg(ac, fromGroup, "游戏长时间未开始，报名终止");
			ostringstream ss;
			ss << "C:\\Users\\Administrator\\Desktop\\CQP\\data\\ingame\\" << fromGroup;
			string s_tmp = ss.str();
			remove(s_tmp.c_str());
		}
		return EVENT_IGNORE;
	}
	if (game_status[fromGroup] == SIGN && fromQQ==game_player[fromGroup][0] && !strcmp(msg, "开始游戏"))
	{
		if (player_cnt[fromGroup] < 4)
		{
			CQ_sendGroupMsg(ac, fromGroup, "人数不足，无法开始");
			return EVENT_IGNORE;
		}
		game_start(fromGroup);
		return EVENT_IGNORE;
	}
	if (game_status[fromGroup] == SIGN && !strcmp(msg, "报名"))
	{
		if (in_game[fromQQ])
		{
			AT(fromGroup, fromQQ, "您已加入一场游戏");
			return EVENT_IGNORE;
		}
		game_player[fromGroup].push_back(fromQQ);
		in_game[fromQQ] = fromGroup;
		player_cnt[fromGroup]++;
		player_quit[fromQQ] = false;
		AT(fromGroup, fromQQ, "报名成功");
		if (player_cnt[fromGroup] >= 6)
		{
			player_cnt[fromGroup] = 6;
			game_start(fromGroup);
		}
		return EVENT_IGNORE;
	}
	if (in_game[fromQQ] && !strcmp(msg, "退出"))
	{
		if (game_status[fromGroup] == SIGN)
		{
			if (fromQQ == game_player[fromGroup][0])
			{
				game_status[fromGroup] = 0;
				pre_sign[fromGroup] = 0;
				for (int i = 0; i < game_player[fromGroup].size(); i++)
				{
					in_game[game_player[fromGroup][i]] = 0;
				}
				game_player[fromGroup].clear();
				CQ_sendGroupMsg(ac, fromGroup, "发起人退出，报名终止");
				ostringstream ss;
				ss << "C:\\Users\\Administrator\\Desktop\\CQP\\data\\ingame\\" << fromGroup;
				string s_tmp = ss.str();
				remove(s_tmp.c_str());
			}
			else
			{
				in_game[fromQQ] = 0;
				game_player[fromGroup].erase(find(game_player[fromGroup].begin(), game_player[fromGroup].end(),fromQQ));
				player_cnt[fromGroup]--;
				AT(fromGroup,fromQQ,"取消报名");
			}
		}
		else
		{
			quit_game(fromQQ);
		}
		return EVENT_IGNORE;
	}
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=4 讨论组消息
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t msgId, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=101 群事件-管理员变动
* subType 子类型，1/被取消管理员 2/被设置管理员
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=102 群事件-群成员减少
* subType 子类型，1/群员离开 2/群员被踢 3/自己(即登录号)被踢
* fromQQ 操作者QQ(仅subType为2、3时存在)
* beingOperateQQ 被操作QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=103 群事件-群成员增加
* subType 子类型，1/管理员已同意 2/管理员邀请
* fromQQ 操作者QQ(即管理员QQ)
* beingOperateQQ 被操作QQ(即加群的QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=201 好友事件-好友已添加
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=301 请求-好友添加
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=302 请求-群添加
* subType 子类型，1/他人申请入群 2/自己(即登录号)受邀入群
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}

/*
* 菜单，可在 .json 文件中设置菜单数目、函数名
* 如果不使用菜单，请在 .json 及此处删除无用菜单
*/
CQEVENT(int32_t, __menuA, 0)() {
	MessageBoxA(NULL, "这是menuA，在这里载入窗口，或者进行其他工作。", "" ,0);
	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	MessageBoxA(NULL, "这是menuB，在这里载入窗口，或者进行其他工作。", "" ,0);
	return 0;
}
