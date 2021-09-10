#include "stdafx.h"
#include "Diagram.h"

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

void Diagram::AddLine(const std::string& aLine)
{
	if (ParseChannel(aLine))
		return;

	if (ParseMessage(aLine))
		return;

}

std::vector<DrawCommand*> Diagram::Finalize()
{
	std::vector<DrawCommand*> out;
	const static int channelWidth = 120;
	const static int arrowWidth = 5;
	const static int arrowLength = 10;

	int x = 70;
	for (size_t channelIndex = 0; channelIndex < myChannels.size();channelIndex++)
	{
		Channel& channel = myChannels[channelIndex];

		int y = 20;
		int endofChannel = y;
		int startOfChannel = y;

		for (Node& node : channel.myNodes)
		{
			switch (node.myType)
			{
			case Node::Type::Empty:
				break;
			case Node::Type::Target:
				endofChannel = y + 2 + arrowWidth;
				break;
			case Node::Type::NewChannel:
				out.push_back(new BoxCommand({x - 50,y - 10},{ x + 50,y + 10},V4F(0,0,0,1),false,Canvas::Patterns::Solid,0.f));
				out.push_back(new TextCommand(channel.myName,{x - Canvas::MeasureString(channel.myName).x / 2,y + 7},V4F(0,0,0,1),1.f));
				endofChannel = y + 10;
				startOfChannel = y + 10;
				break;
			case Node::Type::Arrow:
			{
				int other = x + (node.arrow.myTarget - channelIndex) * channelWidth;
				out.push_back(new LineCommand({x + (node.arrow.myLeftEnabled ? arrowLength : 0), y}, {other - (node.arrow.myRightEnabled ? arrowLength : 0), y}, V4F(0, 0, 0, 1), Canvas::Patterns::Solid, 0.2f));
				if (node.arrow.myLeftEnabled)
				{
					out.push_back(new LineCommand({x, y}, {x + arrowLength, y - arrowWidth}, V4F(0, 0, 0, 1), Canvas::Patterns::Solid, 0.3f));
					out.push_back(new LineCommand({x, y}, {x + arrowLength, y + arrowWidth}, V4F(0, 0, 0, 1), Canvas::Patterns::Solid, 0.3f));
					out.push_back(new LineCommand({x + arrowLength, y + arrowWidth}, {x + arrowLength, y - arrowWidth}, V4F(0, 0, 0, 1), Canvas::Patterns::Solid, 0.3f));
				}

				if (node.arrow.myRightEnabled)
				{
					out.push_back(new LineCommand({other - arrowLength, y - arrowWidth}, {other, y}, V4F(0, 0, 0, 1), Canvas::Patterns::Solid, 0.3f));
					out.push_back(new LineCommand({other - arrowLength, y + arrowWidth}, {other, y}, V4F(0, 0, 0, 1), Canvas::Patterns::Solid, 0.3f));
					out.push_back(new LineCommand({other - arrowLength, y + arrowWidth}, {other - arrowLength, y - arrowWidth}, V4F(0, 0, 0, 1), Canvas::Patterns::Solid, 0.3f));
				}

				Canvas::Size size = Canvas::MeasureString(node.arrow.myText);

				out.push_back(new TextCommand(node.arrow.myText,{(x + other - size.x) / 2,y - 2},V4F(0,0,0,1),0.7f));

			}
				endofChannel = y + 2 + arrowWidth;
				break;
			default:
				break;
			}

			y += 30;
		}

		out.push_back(new LineCommand({x, startOfChannel},{x, endofChannel},V4F(0,0,0,1),Canvas::Patterns::Dashed, -0.5f));
		out.push_back(new LineCommand({x - arrowWidth, endofChannel},{x + arrowWidth, endofChannel},V4F(0,0,0,1),Canvas::Patterns::Solid, -0.4f));
		out.push_back(new LineCommand({x - arrowWidth, endofChannel},{x, endofChannel + arrowLength/2},V4F(0,0,0,1),Canvas::Patterns::Solid, -0.4f));
		out.push_back(new LineCommand({x + arrowWidth, endofChannel},{x, endofChannel + arrowLength/2},V4F(0,0,0,1),Canvas::Patterns::Solid, -0.4f));

		x += channelWidth;
	}

	return out;
}

bool Diagram::ParseMessage(const std::string& aLine)
{
	size_t dash = aLine.find("-", 1);
	if (dash == aLine.npos)
		return false;

	size_t colon = aLine.find(":", dash);
	if (colon == aLine.npos)
		return false;

	if (colon > aLine.length() - 2)
		return false;

	bool leftEnabled = aLine[dash - 1] == '<';
	bool rightEnabled = aLine[dash + 1] == '>';
	if (leftEnabled && dash == 1)
		return false;

	if (rightEnabled && dash + 2 == colon)
		return false;

	size_t channelA = GetChannel(aLine.substr(0,dash - (leftEnabled ? 1 : 0)));
	size_t channelB = GetChannel(aLine.substr(dash + (rightEnabled ? 2 : 1),colon - (dash + (rightEnabled ? 2 : 1))));

	if (channelA == channelB)
		return false;

	if (channelB < channelA)
	{
		std::swap(channelA,channelB);
		std::swap(leftEnabled,rightEnabled);
	}

	{
		Node node;
		node.myType = Node::Type::Arrow;

		node.arrow.myLeftEnabled = leftEnabled;
		node.arrow.myRightEnabled = rightEnabled;
		node.arrow.myTarget = channelB;
		node.arrow.myText = aLine.substr(colon+1);
		myChannels[channelA].myNodes.push_back(node);
	}

	{
		Node node;
		node.myType = Node::Type::Target;
		myChannels[channelB].myNodes.push_back(node);
	}

	PadChannels();

	return true;
}

bool Diagram::ParseChannel(const std::string& aLine)
{
	if (aLine.size() < 3)
		return false;

	if (aLine.front() != '[')
		return false;

	if (aLine.back() != ']')
		return false;

	GetChannel(aLine.substr(1,aLine.size() - 2));

	return true;
}

size_t Diagram::GetChannel(const std::string& aName)
{
	for (size_t i = 0; i < myChannels.size(); i++)
	{
		if (myChannels[i].myName == aName)
		{
			return i;
		}
	}
	myChannels.push_back(Channel{aName,{}});
	Channel& channel = myChannels.back();
	while (channel.myNodes.size() < myChannels[0].myNodes.size() - 1)
	{
		Node node;
		node.myType = Node::Type::Empty;
		channel.myNodes.push_back(node);
	}
	{
		Node node;
		node.myType = Node::Type::NewChannel;
		node.newChannel.myName = aName;
		channel.myNodes.push_back(node);
	}

	return myChannels.size() - 1;
}

void Diagram::PadChannels()
{
	size_t max = 0;
	for (Channel& channel : myChannels)
	{
		max = std::max(max,channel.myNodes.size());
	}
	for (Channel&	channel : myChannels)
	{
		while (channel.myNodes.size() < max)
		{
			Node node;
			node.myType = Node::Type::Empty;
			channel.myNodes.push_back(node);
		}
	}
}

Diagram::Node::Node() : myType(Type::Empty)
{
}

Diagram::Node::~Node()
{
}

Diagram::Node::Node(const Node& aOther)
{
	myType		= aOther.myType;
	empty		= aOther.empty;
	target		= aOther.target;
	newChannel	= aOther.newChannel;
	arrow		= aOther.arrow;
}
