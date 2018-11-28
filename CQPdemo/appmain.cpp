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
vector <string> URL;

int ac = -1; //AuthCode 调用酷Q的方法时需要用到
bool enabled = false;


/* 
* 返回应用的ApiVer、Appid，打包后将不会调用
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}

void get_url()
{
	ifstream ifs("C:\\Users\\Administrator\\Desktop\\CQP\\data\\url.txt");
	int num;
	string tmp;
	while (ifs >> num)
	{
		ifs >> tmp;
		URL.push_back(tmp);
	}
	ifs.close();
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
	get_url();
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

string make_card(int num, int ID) //[CQ:share,url={1},title={2},content={3},image={4}]
{
	ostringstream ss;
	ss << "[CQ:share,url=" << URL[ID] << ",title=" << num << ",content=点击查看大图,image=" << URL[ID] << "]";
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
