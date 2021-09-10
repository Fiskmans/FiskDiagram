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
	bool ParseBox(const std::string& aLine);
	bool ParseTrash(const std::string& aLine);

	size_t GetChannel(const std::string& aName);

	void PadChannels();

	struct Node
	{
		Node();

		enum class Type
		{
			Empty,
			Target,
			BoxTarget,
			NewChannel,
			Arrow,
			Box

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
		struct Box
		{
			size_t		myTarget;
			std::string myText;
		};

		Empty		empty;
		Empty		target;
		Empty		boxTarget;
		NewChannel	newChannel;
		Arrow		arrow;
		Box			box;

	};

	struct Channel
	{
		std::string myName;
		bool myHasNamePlate;
		std::vector<Node> myNodes;
	};

	std::vector<Channel> myChannels;

};
