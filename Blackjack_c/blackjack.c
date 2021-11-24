#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include<ctype.h>
#include <time.h>
#include<conio.h>

//#include "sort.h"

#ifndef MIN(X, Y)
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#endif // !MIN

#ifndef MAX(X, Y)
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#endif // !MAX

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

/****************************************************************************************************************************************/
#define BJ_MAX_HANDS_PER_PLAYER 4
//#define ONLY_VALUE_CARDS 8
//#define NO_CLS 1

/****************************************************************************************************************************************/

static void swap_g(void* a, void* b, size_t size_m)
{
	size_t temp, written = 0;
	while (MIN(sizeof(size_t), size_m - written) > 0)
	{
		if ((size_m - written) < sizeof(size_t))
		{
			memcpy(&temp, (((char*)a) + written), (size_m - written));
			memcpy((((char*)a) + written), (((char*)b) + written), (size_m - written));
			memcpy((((char*)b) + written), &temp, (size_m - written));
			break;
		}
		temp = *(size_t*)(((char*)a) + written);
		*(size_t*)(((char*)a) + written) = *(size_t*)(((char*)b) + written);
		*(size_t*)(((char*)b) + written) = temp;
		written += MIN(sizeof(size_t), size_m - written);
	}
}

static void randomize(void* _arr, size_t _num_elem, size_t _size_m) //Fisher–Yates shuffle algorithm
{
	size_t i, j;
	if (_num_elem <= 1) return;
	srand((unsigned int)time(NULL));
	_num_elem *= _size_m;
	for (i = _num_elem - _size_m; i > 0; i -= _size_m)
	{
		j = (rand() % ((i / _size_m) + 1)) * _size_m;
		swap_g((((char*)_arr) + i), (((char*)_arr) + j), _size_m);
	}
}
/****************************************************************************************************************************************/
static void print_bj_logo()
{
}



typedef unsigned char BYTE;
enum won_lost{won = 1, tie = 0, lost = -1};
enum bool{ false = 0, true = 1 };
enum value { ace = 1, two, three, four, five, six, seven, eight, nine, ten, jack, queen, king };
enum color { black = 0, red };
enum suit { clubs = 0, diamond, hearts, spades };

typedef struct card
{
	enum value value;
	enum color color;
	enum suit suit;
}card;

typedef struct deck
{
	struct card cards[52 * 8];  // the fonal dek has 8 normal deks, 52*8 cards
	size_t num_cards;
}deck;

typedef struct hand
{
	card cards[15];
	int num_cards;
	enum bool still_playing;
	enum bool stay;
	enum won_lost won_lost;
	double bet_ammount;
}hand;

typedef struct player
{
	size_t id; // dealer in id 0
	hand hands[BJ_MAX_HANDS_PER_PLAYER];
	size_t num_hands;
	double balance;
}player;


typedef struct table
{
	size_t player_id_count;
	struct deck deck;
	struct player dealer; // dealer in id 0
	struct player players[7];
	size_t num_players;
	//struct bet bets[7*10];
	//size_t num_bets;
};

static void print_card(card*);

static void print_deck(deck* _deck)
{
	for (size_t i = 0; i < _deck->num_cards; i++)
	{
		print_card(&_deck->cards[i]);
		printf("\n");
	}
}

/****************************************************************************************************************************************/
static void shuffle_deck(struct deck* _deck)
{
	randomize(_deck->cards, _deck->num_cards, sizeof(card));
}


static void init_deck(struct deck* _deck)
{
	size_t i, j, k, color, arr_count = 0;
	_deck->num_cards = 52 * 8;
	for (k = 0; k < 8; k++)
	{
		for (i = 1; i <= 13; i++)
		{
			color = 1;
			for (j = 0; j < 4; j++)
			{
				_deck->cards[arr_count].value = i;
				_deck->cards[arr_count].color = color;
				_deck->cards[arr_count].suit = j;
				color++;
				if (color == 2)
				{
					color = 0;
				}
				arr_count++;
			}
		}
	}
	//print_deck(_deck);
	shuffle_deck(_deck);
}

static struct card pick_card(struct deck* _deck)
{
	struct card card_temp;
	_deck->num_cards--;
	card_temp = _deck->cards[_deck->num_cards];
	return card_temp;
}
/****************************************************************************************************************************************/

static void init_hand(hand *_hand)
{
	_hand->bet_ammount = 0;
	_hand->num_cards = 0;
	_hand->stay = false;
	_hand->still_playing = true;
}

static void init_player(struct player* _player, size_t _id, double _money)
{
	_player->id = _id;
	_player->balance = _money;
	_player->num_hands = 1;
	init_hand(&_player->hands[0]);
}

static double make_bet(struct player* _player, double _money)
{

	_player->hands[0].bet_ammount = MIN(_player->balance, _money);
	_player->balance -= _player->hands[0].bet_ammount;
	//printf("balance: %lf   money: %lf    bet: %lf\n\n", _player->balance, _money, _player->bet_ammount);
	return _player->hands[0].bet_ammount;
}

static struct card* player_pick_card(hand *_hand, struct table* _table)
{
	_hand->cards[_hand->num_cards] = pick_card(&_table->deck);

	#ifdef ONLY_VALUE_CARDS
		_hand->cards[_hand->num_cards].value = ONLY_VALUE_CARDS;
	#endif // ONLY_VALUE_CARDS

	_hand->num_cards += 1;
	return &_hand->cards[_hand->num_cards - 1];
}

static void add_player(struct table* _table, double _money)
{
	init_player(&_table->players[_table->num_players], _table->player_id_count, _money);
	_table->num_players++;
	_table->player_id_count++;
}

static void init_table(struct table* _table)
{
	_table->player_id_count = 1;
	_table->num_players = 0;
	init_deck(&_table->deck);
	init_player(&_table->dealer, 0, 0.00);
}

static int still_playing_players(struct table* _table)
{
	size_t i, j;
	int count = 0;
	for (i = 0; i < _table->num_players; i++)
	{
		for (j = 0; j < _table->players[i].num_hands; j++)
		{
			if (_table->players[i].hands[j].still_playing == true)
			{
				count++;
			}
		}
	}
	return count;
}

static int pending_hands(struct table* _table)
{
	size_t i, j;
	int count = 0;
	for (i = 0; i < _table->num_players; i++)
	{
		for (j = 0; j < _table->players[i].num_hands; j++)
		{
			if (_table->players[i].hands[j].still_playing == true && _table->players[i].hands[j].stay == false)
			{
				count++;
			}
		}
	}
	return count;
}

static void print_card(card *_card)
{
	switch (_card->value)
	{
	case ace:
		printf("Ace");
		break;
	case jack:
		printf("Jack");
		break;
	case queen:
		printf("Queen");
		break;
	case king:
		printf("King");
		break;
	default:
		printf("%d", _card->value);
		break;
	}
	printf("\t");
	switch (_card->suit)
	{
	case clubs:
		printf("clubs");
		break;
	case diamond:
		printf("diamond");
		break;
	case hearts:
		printf("hearts");
		break;
	case spades:
		printf("spades");
		break;
	default:
		printf("%d", _card->suit);
		break;
	}
	printf("\t");
	switch (_card->color)
	{
	case black:
		printf("black");
		break;
	case red:
		printf("red");
		break;
	default:
		printf("%d", _card->color);
		break;
	}
}

static void print_hand(hand *_hand)
{
	size_t i;

	//printf("Stay: %d\n", _hand->stay);
	//printf("still_playing: %d\n", _hand->still_playing);
	//printf("won_lost: %d\n", _hand->won_lost);
	//printf("num_cards: %d\n", _hand->num_cards);

	printf("Bet ammount: %.2lf $\n", _hand->bet_ammount);
	printf("Hand points: %d\n", hand_points(_hand));
	printf("Stage game: ");
	switch (_hand->still_playing)
	{
	case false:
		if (_hand->won_lost == won)
		{
			printf("WON ");
			if (hand_points(_hand) == 21) printf("%.2lf $", _hand->bet_ammount * 2/3);
			else printf("%g $", _hand->bet_ammount);
		}
		else if (_hand->won_lost == lost) printf("LOST %.2lf $", _hand->bet_ammount);
		else if (_hand->won_lost == tie) printf("TIE");
		break;
	default:
		printf("STILL PLAYING");
		break;
	}
	printf("\n");
	printf("\nValue\tSuit\tcolor\n\n");
	for (i = 0; i < _hand->num_cards; i++)
	{
		print_card(&_hand->cards[i]);
		printf("\n");
	}
}

static void print_hands(player* _player)
{
	size_t i;
	for (i = 0; i < _player->num_hands; i++)
	{
		printf("<--------%d-------->\n", i);
		print_hand(&_player->hands[i]);
		printf("\n");
	}
}


static void print_player_info(player* _player)
{
	size_t i;
	printf("*************************Player %d **************************\n", _player->id);
	printf("ID: %u\n", _player->id);
	printf("Balance: %.2lf $\n", _player->balance);
	print_hands(_player);
}

static void print_dealer_info(player* _player)
{
	size_t i;
	printf("*************************Dealer %d **************************\n", _player->id);
	printf("ID: %u\n", _player->id);
	printf("Balance: %.2lf $\n", _player->balance);
	print_hands(_player);
}

static void won_lost_routine(struct player* _player, struct table* _table);

static void print_all_players_info(struct table* _table)
{
	size_t i;
	#ifndef NO_CLS
		system("Cls");
	#endif // NO_CLS
	print_bj_logo();
	printf("-------------------------------Players info-----------------------------\n");
	print_dealer_info(&_table->dealer);
	for (i = 0; i < _table->num_players; i++)
	{
		won_lost_routine(&_table->players[i], _table);
		print_player_info(&_table->players[i]);
	}
	printf("-----------------------------------------------------------------------\n\n");
}
/****************************************************************************************************************************************/
static int hand_points(hand* _hand)
{
	size_t i, aces = 0;
	int points = 0, points_to_21 = 0;
	for (i = 0; i < _hand->num_cards; i++)
	{
		if (_hand->cards[i].value == ace)
		{
			aces++;
		}
		else if (_hand->cards[i].value >= 11 && _hand->cards[i].value <= 13)
		{
			points += 10;
		}
		else
		{
			points += _hand->cards[i].value;
		}
	}
	points_to_21 = 21 - points;
	if (aces > 0)
	{
		if ((11 + aces - 1) <= points_to_21)
		{
			points += 11 + aces - 1;
		}
		else
		{
			points += aces;
		}
	}
	return points;
}

static int won_lost_hand(hand* _hand, struct table* _table) //0=lost, 1=won, 2=tie, -1=still in the game
{
	int hand_points_ = hand_points(_hand);
	int dealer_points = hand_points(&_table->dealer.hands[0]);
	enum bool hand_stay = _hand->stay;
	enum bool dealer_stay = _table->dealer.hands[0].stay;
	if (hand_points_ == 21)
	{
		_hand->stay = true;
		_hand->won_lost = won;
		return 1;
	}
	else if (hand_points_ > 21)
	{
		_hand->won_lost = lost;
		_hand->stay = true;
		return 0;
	}
	else if (dealer_points > 21)
	{
		_hand->won_lost = won;
		_table->dealer.hands[0].stay = true;
		_hand->stay = true;
		return 1;
	}
	else if (hand_stay == true && dealer_stay == true)
	{
		if (hand_points_ > dealer_points)
		{
			_hand->won_lost = won;
			_hand->stay = true;
			return 1;
		}
		else if (dealer_points > hand_points_)
		{
			_hand->won_lost = lost;
			return 0;
		}
		else
		{
			_hand->won_lost = tie;
			return 2;
		}
	}
	else
	{
		return -1;
	}
}




static double won_lost_ammount_hand(hand* _hand, struct table* _table)
{
	int hand_points_ = hand_points(_hand);
	int dealer_points = hand_points(&_table->dealer.hands[0]);
	enum bool hand_stay = _hand->stay;
	enum bool dealer_stay = _table->dealer.hands[0].stay;
	if (hand_points_ == 21)
	{
		_hand->stay = true;
		return _hand->bet_ammount * 2 / 3;
	}
	else if (hand_points_ > 21)
	{
		_hand->stay = true;
		return -_hand->bet_ammount;
	}
	else if (dealer_points > 21)
	{
		_table->dealer.hands[0].stay = true;
		return _hand->bet_ammount;
	}
	else if (hand_stay == dealer_stay == true)
	{
		if (hand_points_ > dealer_points)
		{
			return _hand->bet_ammount;
		}
		else if (dealer_points > hand_points_)
		{
			return -_hand->bet_ammount;
		}
		else
		{
			return 0.00;
		}
	}
	else
	{
		return 0.00;
	}
}

static void won_lost_routine(struct player* _player, struct table* _table)
{
	//print_player_info(_player);
	size_t i;
	for (i = 0; i < _player->num_hands; i++)
	{
		//printf("won_lost_routine hand : %d\n\n", i);
		switch (won_lost_hand(&_player->hands[i], _table))
		{
		case -1:
			_player->hands[i].still_playing = true;
			break;
		case 0:
			_player->hands[i].still_playing = false;
			//printf("        YOU LOST %lf\n\n", won_lost_ammount_hand(&_player->hands[i], _table));
			//_player->bet_ammount = 0.00;
			break;
		case 1:
			_player->hands[i].still_playing = false;
			//printf("        YOU WON %lf$\n\n", won_lost_ammount_hand(&_player->hands[i], _table));
			_player->balance += _player->hands[i].bet_ammount + won_lost_ammount_hand(&_player->hands[i], _table);
			//_player->bet_ammount = 0.00;
			break;
		case 2:
			_player->hands[i].still_playing = false;
			//printf("        IT'S A TIE\n\n");
			_player->balance += _player->hands[i].bet_ammount;
			//_player->bet_ammount = 0.00;
			break;
		}
	}
}
/****************************************************************************************************************************************/
static void give_first_2_cards(struct table* _table)
{
	size_t i;
	struct card* temp_card;
	shuffle_deck(&_table->deck);
	//print_all_players_info(_table);
	#ifndef NO_CLS
		system("Cls");
	#endif // NO_CLS
	printf("////////////////////////////////////First 2 cars/////////////////////////////////////////////////\n");
	temp_card = player_pick_card(&_table->dealer.hands[0], _table); //dealer card
	print_dealer_info(&_table->dealer);
	for (i = 0; i < _table->num_players; i++)	//Piking first card stage
	{
		if (_table->players[i].hands[0].bet_ammount == 0.00) continue;
		temp_card = player_pick_card(&_table->players[i].hands[0], _table);
		temp_card = player_pick_card(&_table->players[i].hands[0], _table);
		won_lost_routine(&_table->players[i], _table);
		print_player_info(&_table->players[i]);
	}
	printf("////////////////////////////////////////////////////////////////////////////////////////////////\n\n");
}


static void split_hand(hand *_hand_to_split, hand *_hand_2)
{
	if (_hand_to_split->num_cards != 2) return;
	_hand_2->num_cards = 1;
	_hand_to_split->num_cards = 1;

	_hand_2->cards[0] = _hand_to_split->cards[1];

	_hand_2->bet_ammount = _hand_to_split->bet_ammount;
	_hand_2->stay = false;
	_hand_2->still_playing = true;
}

static void give_more_cards(struct table* _table)
{
	size_t j, i;
	char char_temp_1;
	struct card* temp_card;
	while (pending_hands(_table) > 0)
	{
		for (j = 0; j < _table->num_players; j++)
		{
			for (i = 0; i < _table->players[j].num_hands; i++)
			{
				if (_table->players[j].hands[i].bet_ammount == 0.00 || _table->players[j].hands[i].stay == true || _table->players[j].hands[i].still_playing == false/* || won_lost_hand(&_table->players[j].hands[i], _table) == -1*/) continue;
				print_all_players_info(_table);
				//print_player_info(&_table->players[j]);
				while (1)
				{
					if (_table->players[j].hands[i].num_cards == 2 && _table->players[j].hands[i].cards[0].value == _table->players[j].hands[i].cards[1].value
						&& _table->players[j].num_hands < BJ_MAX_HANDS_PER_PLAYER && (_table->players[j].balance / _table->players[j].hands[0].bet_ammount) >= 1)
					{
						printf("%s", KRED);
						printf("\nPlayer number %d do you want to split hand nr %d ? (Y/N): ", _table->players[j].id, i);
						printf("%s", KNRM);
						//print_hand(&_table->players[j].hands[i]);
						scanf("%c", &char_temp_1); getchar();
						if (char_temp_1 == 'y' || char_temp_1 == 'Y')
						{
							split_hand(&_table->players[j].hands[i], &_table->players[j].hands[_table->players[j].num_hands]);
							_table->players[j].balance -= _table->players[j].hands[0].bet_ammount;
							_table->players[j].num_hands++;
							//break;
						}
					}
					print_all_players_info(_table);
					printf("Player number %d pick a card for hand nr %d (Y/N): ", _table->players[j].id, i);
					scanf("%c", &char_temp_1); getchar();

					if (char_temp_1 == 'y' || char_temp_1 == 'Y')
					{
						temp_card = player_pick_card(&_table->players[j].hands[i], _table);
						break;
					}
					else if (char_temp_1 == 'n' || char_temp_1 == 'N')
					{
						_table->players[j].hands[i].stay = true;
						break;
					}
					else
					{
						printf("Invalid option!\n");
					}
				}
				print_all_players_info(_table);
			}
		}
	}
}

static int dealer_pick_until(struct table* _table) //return dealer hand points
{
	while (hand_points(&_table->dealer.hands[0]) < 17)
	{
		printf("dealer cards: %d\n\n", _table->dealer.hands[0].num_cards);
		player_pick_card(&_table->dealer.hands[0], _table);
	}
	_table->dealer.hands[0].stay = true;
	
	return hand_points(&_table->dealer.hands[0]);
}

static void new_round(struct table* _table)
{
	size_t i;
	init_player(&_table->dealer, _table->dealer.id, _table->dealer.balance);
	for (i = 0; i < _table->num_players; i++)
	{
		init_player(&_table->players[i], _table->players[i].id, _table->players[i].balance);
	}
}
/**************************************************************************************************************************************************************/

static int main_loop()
{
	size_t i, j, uint_temp_1;
	char char_temp_1;
	double default_money = 2000.00, temp_bet;
	struct table bj_table;
	struct card* temp_card;
	init_table(&bj_table);
	//print_deck(&bj_table.deck);
	while (1) //main loop
	{
		print_bj_logo();
		printf("Insert number of players (max 7): ");
		scanf("%u", &uint_temp_1); getchar();
		for (i = 0; i < MIN(uint_temp_1, 7); i++)
		{
			add_player(&bj_table, default_money);
		}
		while (1) //rounds loop
		{
			for (i = 0; i < bj_table.num_players; i++)	//betting stage
			{
				print_all_players_info(&bj_table);
				//print_player_info(&bj_table.players[i]);
				printf("Player number %d put your bet: ", bj_table.players[i].id);
				//printf("Bet ammount: ");
				scanf("%lf", &temp_bet); getchar();
				make_bet(&bj_table.players[i], temp_bet);
			}
			give_first_2_cards(&bj_table);
			give_more_cards(&bj_table);
			if(still_playing_players(&bj_table) > 0) dealer_pick_until(&bj_table);

			print_all_players_info(&bj_table);

			while (1)
			{
				printf("\n\nNew round? (Y/N): ");
				scanf("%c", &char_temp_1); getchar();

				if (char_temp_1 == 'y' || char_temp_1 == 'Y')
				{
					break;
				}
				else if (char_temp_1 == 'n' || char_temp_1 == 'N')
				{
					return 0;
					break;
				}
				else printf("invalid option!\n");
			}
			printf("\n\nNew round? (Y/N)\n");
			new_round(&bj_table);
		}
	}
	return 0;
}
