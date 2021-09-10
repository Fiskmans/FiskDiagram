#pragma once
#include "DrawCommand.h"

class Diagram
{
public:
	void AddLine(const std::string& aLine);

	std::vector<DrawCommand*> Finalize();
private:

	bool ParseMessage(const std::string& aLine);
	bool ParseChannel(const std::string& aLine);

	size_t GetChannel(const std::string& aName);

	void PadChannels();

	struct Node
	{
		Node();
		~Node();
		Node(const Node& aOther);

		enum class Type
		{
			Empty,
			Target,
			NewChannel,
			Arrow

		} myType;

		struct Arrow
		{
			bool		myLeftEnabled;
			bool		myRightEnabled;
			size_t		myTarget;
			std::string myText;
		};
		struct Empty
		{
		};
		struct NewChannel
		{
			std::string myName;
		};

		Empty		empty;
		Empty		target;
		NewChannel	newChannel;
		Arrow		arrow;
	};

	struct Channel
	{
		std::string myName;
		std::vector<Node> myNodes;
	};

	std::vector<Channel> myChannels;

};
