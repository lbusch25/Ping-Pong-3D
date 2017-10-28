
#include "BaseApp.h"

namespace basicgraphics {

	class ExampleApp : public BaseApp {
		public:
			ExampleApp(int argc, char** argv, std::string windowName, int windowWidth, int windowHeight);
			~ExampleApp();

		private:
			std::unique_ptr<Box> _box;
			float _angle;


			void onRenderGraphics() override;
			void onEvent(std::shared_ptr<Event> event) override;
	};
}