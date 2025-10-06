//Author : WrongAnswer99
#include <string>
namespace attr {
#define obj(x) namespace x
#define tag(x) const std::string x=#x;
	//obj(player) {tag(hp)tag(weapon) }
	namespace gui {
		enum Statu { normal = 0, over = 1, focus = 2 };
		enum TextJustification { Left = 0, Right = 2, Top = 0, Bottom = 2, Mid = 1 };
		enum InputTypeLimit { String = 0, Int = 1, Float = 2 };
		tag(AreaPath);
		tag(ButtonId);
		tag(InputId);
		tag(MouseButton);

		tag(ButtonPressed);
		tag(ButtonPath);

		tag(InputGainFocus);
		tag(InputPath);

		tag(InputLoseFocus);
	}
	namespace ani {
		tag(AnimationEnd);
	}
#undef obj
#undef tag
}