#include "menu_item.h"
#include <vector>
#include <memory>
#include <iostream>

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

		cout<<"Неизвестная команда\n";
	}
private:
	vector<unique_ptr<MenuItem>> items;

};