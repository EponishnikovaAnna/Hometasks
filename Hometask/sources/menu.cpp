#include "menu_item.h"
#include <vector>
#include <memory>
#include <iostream>
#include "loger.h"

using namespace std;

class Menu
{
public:
	void addItem(unique_ptr<MenuItem> item)
	{
		items.push_back(move(item));
	}

	void execute(const string& command)
	{
		for(auto& item: items){
			if(item->getCommandName() == command){
				item->action();
				return;
			}
		}
		LOG_ERROR("Menu::execute: unknown command");

		cout<<"Неизвестная команда\n";
	}
private:
	vector<unique_ptr<MenuItem>> items;

};