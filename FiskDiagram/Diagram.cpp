#include "stdafx.h"
#include "Diagram.h"
#include <sstream>
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

void Diagram::AddLine(const std::string& aLine)
{
	if (aLine.starts_with("#") ||aLine.starts_with("//"))
		return;

	if (ParseChannel(aLine))
		return;

	if (ParseMessage(aLine))
		return;

	if (ParseBox(aLine))
		return;

	if(ParseTrash(aLine))
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
				out.push_back(new BoxCommand({x - 50,y - 15},{ x + 50,y + 12},V4F(0,0,0,1),false,Canvas::Patterns::Solid,0.f));
				out.push_back(new TextCommand(channel.myName,{x - Canvas::MeasureString(channel.myName).x / 2,y + 7},V4F(0,0,0,1),1.f));
				endofChannel = y + 12;
				startOfChannel = y + 12;
				break;
			case Node::Type::Arrow:
			{

				int other = x + static_cast<int>(node.arrow.myTarget - channelIndex) * channelWidth;
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

				out.push_back(new TextCommand(node.arrow.myText,{x + 5 + arrowLength,y - 2},V4F(0,0,0,1),0.7f));

				if (node.arrow.myClarifierColumn != 0)
				{
					size_t startCount = out.size();

					const static int byteSize = 10;
					int clarifier = x + static_cast<int>(node.arrow.myClarifierColumn - channelIndex) * channelWidth;
					
					Canvas::Size arrowTextSize = Canvas::MeasureString(node.arrow.myText + ":");
					
					out.push_back(new TextCommand(node.arrow.myText + ":", {clarifier, y - 12},V4F(0,0,0,1),0.7f));

					clarifier += arrowTextSize.x + 5;


					size_t total = clarifier;
					for (auto& label : node.arrow.myClarifierContent)
					{
						total += label.myMinSize * byteSize;
					}

					for (size_t i = 0; i < node.arrow.myClarifierContent.size(); i++)
					{
						auto& label = node.arrow.myClarifierContent[i];
						out.push_back(new BoxCommand({clarifier, y - 30}, {clarifier + static_cast<int>(label.myMinSize) * byteSize, y - 10}, V4F(0, 0, 0, 1), false, Canvas::Patterns::Solid, 1.f));
						Canvas::Size textSize = Canvas::MeasureString(label.myLabel);

						if (label.myMaxSize.empty())
						{
							out.push_back(new TextCommand(std::to_string(label.myMinSize), {clarifier + 2, y - 31}, V4F(0, 0, 0, 1), 0.7f));
						}
						else
						{
							out.push_back(new TextCommand(std::to_string(label.myMinSize) + "-" + label.myMaxSize, {clarifier + 2, y - 31}, V4F(0, 0, 0, 1), 0.7f));
						}
						if (i != 0)
						{
							out.push_back(new LineCommand({clarifier,y-30},{clarifier,y - 31 - textSize.y * 3 / 4},V4F(0,0,0,1),Canvas::Patterns::Solid,0.5f));
						}

						int labelHeight = y - 6 + textSize.y * static_cast<int>(node.arrow.myClarifierContent.size() - i);
						out.push_back(new TextCommand(label.myLabel, {static_cast<int>(total) + 5, labelHeight}, V4F(0, 0, 0, 1), 0.7f));
						out.push_back(new BezierCommand(
							{ clarifier + static_cast<int>(label.myMinSize) * byteSize / 2, y - 10},
							{ clarifier + static_cast<int>(label.myMinSize) * byteSize / 2, labelHeight},
							{ clarifier + static_cast<int>(label.myMinSize) * byteSize / 2, labelHeight - textSize.y / 2},
							{ static_cast<int>(total), labelHeight - textSize.y / 2},
							V4F(0,0,0,1),
							Canvas::Patterns::Dotted,
							0.3f
							));
						clarifier += static_cast<int>(label.myMinSize) * byteSize;
					}

					Canvas::Point min = {x + static_cast<int>(node.arrow.myClarifierColumn - channelIndex) * channelWidth - 5, y - 44 - 5};
					Canvas::Point max = {0,0};
					for (size_t i = startCount; i < out.size(); i++)
					{
						Canvas::Point maxPoint = out[i]->Max();
						max.x = std::max(max.x, maxPoint.x);
						max.y = std::max(max.y, maxPoint.y);
					}
					max.x += 5;
					max.y += 5;

					out.push_back(new BoxCommand(min,max,V4F(0,0,0,1),false,Canvas::Patterns::Solid,0.1f));
				}

			}
				endofChannel = y + 2 + arrowWidth;
				break;
			case Node::Type::Box:
			{
				int other = x + static_cast<int>(node.box.myTarget - channelIndex) * channelWidth;

				out.push_back(new BoxCommand({x - 19, y - 9}, {other + 19, y + 9}, V4F(1, 1, 1, 1), true, Canvas::Patterns::Solid, 0.9f));
				out.push_back(new BoxCommand({x - 20, y - 10}, {other + 20, y + 10}, V4F(0, 0, 0, 1), false, Canvas::Patterns::Solid, 1.f));
				out.push_back(new TextCommand(node.box.myText,{ (x + other - Canvas::MeasureString(node.box.myText).x) / 2, y + 7},V4F(0,0,0,1),1.1f));
			}
				break;
			default:
				break;
			}

			y += 30;
		}

		if (endofChannel > startOfChannel)
		{
			out.push_back(new LineCommand({x, startOfChannel},{x, endofChannel},V4F(0.5f,0.5f,0.5f,1),Canvas::Patterns::Dashed, -0.5f));
			out.push_back(new LineCommand({x - arrowWidth, endofChannel},{x + arrowWidth, endofChannel},V4F(0,0,0,1),Canvas::Patterns::Solid, -0.4f));
			out.push_back(new LineCommand({x - arrowWidth, endofChannel},{x, endofChannel + arrowLength/2},V4F(0,0,0,1),Canvas::Patterns::Solid, -0.4f));
			out.push_back(new LineCommand({x + arrowWidth, endofChannel},{x, endofChannel + arrowLength/2},V4F(0,0,0,1),Canvas::Patterns::Solid, -0.4f));
		}

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
		node.arrow.myClarifierColumn = 0;

		{
			std::string content = aLine.substr(colon+1);
			node.arrow.myText = content;

			size_t start = content.find('[');
			if (start != content.npos)
			{
				size_t end = content.find(']',start);
				if (end != aLine.npos)
				{
					node.arrow.myClarifierColumn = myChannels.size();
					std::string clarifier = content.substr(start+1,end-start-1);
					std::stringstream ss(clarifier);
					std::string item;
					while (getline(ss,item,';'))
					{
						std::cout << item << std::endl;

						Node::Label label;
						std::stringstream ss1(item);
						if (!(ss1 >> label.myMinSize >> label.myLabel))
						{
							break;
						}
						ss1 >> label.myMaxSize;
						node.arrow.myClarifierContent.push_back(label);
					}

					node.arrow.myText = content.substr(0,start);
				}
			}
		}
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

	bool quiet = aLine[1] == '*';
	if (quiet && aLine.length() == 3)
	{
		static size_t counter = 0;
		myChannels.push_back(Channel{"PaddingChannel_" + std::to_string(counter++), false, {}});
		return true;
	}

	std::string name = aLine.substr(quiet ? 2 : 1,aLine.size() - (quiet ? 3 : 2));

	for (size_t i = 0; i < myChannels.size(); i++)
	{
		if (myChannels[i].myName == name)
		{
			return false;
		}
	}

	myChannels.push_back(Channel{name, false, {}});
	Channel& channel = myChannels.back();
	while (!myChannels[0].myNodes.empty() && channel.myNodes.size() < myChannels[0].myNodes.size() - ( quiet ? 0 : 1))
	{
		Node node;
		node.myType = Node::Type::Empty;
		channel.myNodes.push_back(node);
	}
	if (!quiet)
	{
		Node node;
		node.myType				= Node::Type::NewChannel;
		node.newChannel.myName	= name;
		channel.myNodes.push_back(node);
		channel.myHasNamePlate	= true;
	}
	PadChannels();

	return true;
}

bool Diagram::ParseBox(const std::string& aLine)
{
	if (!(aLine.starts_with("Box") || aLine.starts_with("box")))
		return false;

	size_t colon = aLine.find(":",3);
	if (colon == aLine.npos)
		return false;

	if (colon > aLine.length() - 2)
		return false;


	if (myChannels.empty())
		return false;

	size_t origin = 0;
	size_t target = myChannels.size()-1;

	if (colon > 3)
	{
		if (aLine[3] != ',')
			return false;


		size_t comma = aLine.find(",",4);
		if (comma != aLine.npos && comma < colon)
		{
			origin = GetChannel(aLine.substr(4, comma - 4));
			target = GetChannel(aLine.substr(comma + 1, colon - comma - 1));
		}
		else
		{
			origin = GetChannel(aLine.substr(4,colon-4));
		}


	}

	{
		{
			Node node;
			node.myType = Node::Type::Box;
			node.box.myTarget = target;
			node.box.myText = aLine.substr(colon+1);

			myChannels[origin].myNodes.push_back(node);
		}

		{
			for (size_t i = origin + 1; i <= target; i++)
			{
				{
					Node node;
					node.myType = Node::Type::BoxTarget;
					myChannels[i].myNodes.push_back(node);
				}
			}
		}
	}

	PadChannels();
	return true;
}

bool Diagram::ParseTrash(const std::string& aLine)
{
	if (myChannels.empty())
		GetChannel("Dummy");
	{
		{
			Node node;
			node.myType			= Node::Type::Box;
			node.box.myTarget	= myChannels.size() - 1;
			node.box.myText		= aLine;

			myChannels[0].myNodes.push_back(node);
		}

		{
			for (size_t i =  1; i < myChannels.size(); i++)
			{
				{
					Node node;
					node.myType = Node::Type::BoxTarget;
					myChannels[i].myNodes.push_back(node);
				}
			}
		}
	}

	PadChannels();
	return true;
}

size_t Diagram::GetChannel(const std::string& aName)
{
	for (size_t i = 0; i < myChannels.size(); i++)
	{
		if (myChannels[i].myName == aName)
		{
			if (!myChannels[i].myHasNamePlate)
			{
				if (myChannels[i].myNodes.back().myType == Node::Type::Empty)
				{
					myChannels[i].myNodes.back().myType = Node::Type::NewChannel;
					myChannels[i].myNodes.back().newChannel.myName = aName;
					myChannels[i].myHasNamePlate = true;
				}
				else
				{
					Node node;
					node.myType				= Node::Type::NewChannel;
					node.newChannel.myName	= aName;
					myChannels[i].myNodes.push_back(node);
					myChannels[i].myHasNamePlate	= true;
					PadChannels();
				}
			}
			return i;
		}
	}
	myChannels.push_back(Channel{aName,false,{}});
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
		channel.myHasNamePlate = true;
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