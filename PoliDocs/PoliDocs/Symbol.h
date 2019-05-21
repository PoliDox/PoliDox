#include <vector>

using namespace std;
class Symbol
{
private:
	int siteId;
	int counter;
	char value;
	vector<int> position;

public:
	Symbol(int siteId, int counter, char value) : siteId(siteId), counter(counter), value(value) {};
	~Symbol();
	void setPosition(std::vector<int> x);
	vector<int> getPosition();
	char getValue();
};

