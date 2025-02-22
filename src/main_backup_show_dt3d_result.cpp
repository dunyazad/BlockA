#include <iostream>

#include <Neon/Neon.h>

int main()
{
	Neon::Application app(1280, 1024);
	Neon::URL::ChangeDirectory("..");

	//app.GetWindow()->UseVSync(false);

	int index = 0;

	app.OnInitialize([&]() {
		auto t = Neon::Time("Initialize");

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		//glFrontFace(GL_CCW);

		glPointSize(10.0f);

		auto scene = app.CreateScene("Scene/Main");
		{
			auto toggler = scene->CreateEntity("Scene/Toggler");
			toggler->AddKeyEventHandler([scene](const Neon::KeyEvent& event) {
				if ((GLFW_KEY_0 <= event.key && GLFW_KEY_9 >= event.key) && GLFW_RELEASE == event.action) {
					auto debugEntities = scene->GetDebugEntities();
					if (event.key - GLFW_KEY_0 < debugEntities.size())
					{
						auto entity = debugEntities[(event.key - GLFW_KEY_0 + 1) % debugEntities.size()];

						auto mesh = entity->GetComponent<Neon::Mesh>(0);
						if (nullptr != mesh)
						{
							mesh->ToggleFillMode();
							cout << "Toggle Fill Mode : " << mesh->GetName() << endl;
						}
					}
				}
				});
		}

#pragma region Camera
		{
			auto entity = scene->CreateEntity("Entity/Main Camera");
			auto transform = scene->CreateComponent<Neon::Transform>("Transform/Main Camera");
			entity->AddComponent(transform);

			auto camera = scene->CreateComponent<Neon::Camera>("Camera/Main", 1280.0f, 1024.0f);
			entity->AddComponent(camera);
			camera->distance = 2.0f;
			camera->centerPosition = glm::vec3(0.0f, 0.0f, 0.0f);
			scene->SetMainCamera(camera);

			auto cameraManipulator = scene->CreateComponent<Neon::CameraManipulator>("CameraManipulator/Main", entity, camera);
			entity->AddComponent(cameraManipulator);
		}
#pragma endregion

#pragma region Light
		{
			auto entity = scene->CreateEntity("Entity/Main Light");

			auto light = scene->CreateComponent<Neon::Light>("Light/Main");
			entity->AddComponent(light);

			light->AddUpdateHandler([scene, light](double now, double timeDelta) {
				auto camera = scene->GetMainCamera();
				light->position = camera->position;
				light->direction = glm::normalize(camera->centerPosition - camera->position);
				});

			scene->SetMainLight(light);
		}
#pragma endregion

#pragma region Guide Axes
		{
			auto entity = scene->CreateEntity("Entity/Axes");
			auto mesh = scene->CreateComponent<Neon::Mesh>("Mesh/Axes");
			entity->AddComponent(mesh);

			mesh->SetDrawingMode(GL_LINES);
			mesh->AddVertex(glm::vec3(0.0f, 0.0f, 0.0f));
			mesh->AddVertex(glm::vec3(10.0f, 0.0f, 0.0f));
			mesh->AddVertex(glm::vec3(0.0f, 0.0f, 0.0f));
			mesh->AddVertex(glm::vec3(0.0f, 10.0f, 0.0f));
			mesh->AddVertex(glm::vec3(0.0f, 0.0f, 0.0f));
			mesh->AddVertex(glm::vec3(0.0f, 0.0f, 10.0f));

			mesh->AddColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			mesh->AddColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			mesh->AddColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			mesh->AddColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			mesh->AddColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
			mesh->AddColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

			mesh->AddIndex(0);
			mesh->AddIndex(1);
			mesh->AddIndex(2);
			mesh->AddIndex(3);
			mesh->AddIndex(4);
			mesh->AddIndex(5);

			auto shader = scene->CreateComponent<Neon::Shader>("Shader/Color", Neon::URL::Resource("/shader/color.vs"), Neon::URL::Resource("/shader/color.fs"));
			entity->AddComponent(shader);
		}
#pragma endregion

		//{
		//	auto entity = scene->CreateEntity("ply");
		//	auto mesh = scene->CreateComponent<Neon::Mesh>("Mesh");
		//	entity->AddComponent(mesh);

		//	mesh->FromPLYFile("C:\\Resources\\3D\\PLY\\first_patch.ply");
		//	mesh->SetFillMode(Neon::Mesh::FillMode::Point);

		//	auto shader = scene->CreateComponent<Neon::Shader>("Shader/Color", Neon::URL::Resource("/shader/color.vs"), Neon::URL::Resource("/shader/color.fs"));
		//	entity->AddComponent(shader);

		//	auto nov = mesh->GetVertexBuffer()->Size();
		//	for (size_t i = 0; i < nov; i++)
		//	{
		//		auto v = mesh->GetVertex(i);
		//		cout << v << endl;
		//	}
		//}


		vector<glm::vec3> points;
		vector<glm::ivec4> indices;

		std::string line;
		std::fstream fs;

		fs.open("C:\\Resources\\3D\\ASCII\\points_optimized.ascii", std::fstream::in);
		while (std::getline(fs, line))
		{
			std::stringstream ss(line);
			float x, y, z;
			char comma;
			ss >> x;
			ss >> comma;
			ss >> y;
			ss >> comma;
			ss >> z;

			points.push_back({ x,y,z });
		}
		fs.close();

		fs.open("C:\\Resources\\3D\\ASCII\\indices.ascii", std::fstream::in);
		while (std::getline(fs, line))
		{
			std::stringstream ss(line);
			long i0, i1, i2, i3;
			char comma;
			ss >> i0;
			ss >> comma;
			ss >> i1;
			ss >> comma;
			ss >> i2;
			ss >> comma;
			ss >> i3;

			indices.push_back({ i0, i1, i2, i3 });
		}
		fs.close();

		for (auto& is : indices)
		{
			auto v0 = points[is.x];
			auto v1 = points[is.y];
			auto v2 = points[is.z];
			auto v3 = points[is.w];

			scene->Debug("tetrahedrons")->AddTriangle(v0, v1, v2);
			/*scene->Debug("tetrahedrons")->AddTriangle(v1, v3, v2);
			scene->Debug("tetrahedrons")->AddTriangle(v0, v3, v1);
			scene->Debug("tetrahedrons")->AddTriangle(v0, v2, v3);*/
		}

		//for (size_t y = 0; y < 50; y++)
		//{
		//	for (size_t x = 0; x < 50; x++)
		//	{
		//		auto xv = Neon::RandomReal<float>(0.0f, 100.0f) - 50.0f;
		//		auto yv = Neon::RandomReal<float>(0.0f, 100.0f) - 50.0f;

		//		//scene->Debug("Spheres")->AddSphere({ xv, yv, 0.0f }, 0.125f, 32, glm::white);
		//		scene->Debug("Spheres")->AddPoint({ xv, yv, 0.0f }, glm::white);
		//	}
		//}



		});





	app.OnUpdate([&](double now, double timeDelta) {
		//glPointSize(cosf(now * 0.005f) * 10.0f + 10.0f);

		//auto t = Neon::Time("Update");

		//fbo->Bind();

		//glClearColor(0.9f, 0.7f, 0.5f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//fbo->Unbind();

		glClearColor(0.3f, 0.5f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		});






	app.OnTerminate([&]() {
		auto t = Neon::Time("Terminate");
		});

	app.Run();

	return 0;
}
