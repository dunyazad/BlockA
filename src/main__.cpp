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

		{
			struct ReconVertex
			{
				float x;
				float y;
				float z;
				float nx;
				float ny;
				float nz;
				float r;
				float g;
				float b;
			};

			vector<ReconVertex> vertices;

			ifstream ifs;
			ifs.open("C:\\Resources\\3D\\PTS\\point.pts", ios::in/* | ios::binary*/);

			if (ifs.is_open())
			{
				size_t count = 0;
				ifs >> count;

				for (size_t i = 0; i < count; i++)
				{
					ReconVertex v;
					ifs >> v.x;
					ifs >> v.y;
					ifs >> v.z;

					ifs >> v.nx;
					ifs >> v.ny;
					ifs >> v.nz;

					ifs >> v.r;
					ifs >> v.g;
					ifs >> v.b;

					vertices.push_back(v);

					scene->Debug("Points")->AddBox({ v.x, v.y, v.z }, 0.01, 0.01, 0.01, { v.r, v.g, v.b, 1.0f });
				}
			}
		}

		return;

		{
			auto entity = scene->CreateEntity("Entity/pc");
			auto mesh = scene->CreateComponent<Neon::Mesh>("Mesh/pc");
			entity->AddComponent(mesh);
		}

		{
			auto entity = scene->CreateEntity("Entity/spot");
			auto mesh = scene->CreateComponent<Neon::Mesh>("Mesh/spot");
			entity->AddComponent(mesh);

			mesh->FromSTLFile(Neon::URL::Resource("/stl/mesh.stl"));
			mesh->FillColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			mesh->RecalculateFaceNormal();

			/*
			{
				float offsetDistance = -0.1f;

				auto noi = mesh->GetIndexBuffer()->Size() / 3;

				vector<glm::vec3> vnormals(mesh->GetVertexBuffer()->Size());
				vector<int> vnormalRefs(mesh->GetVertexBuffer()->Size());

				for (size_t i = 0; i < noi; i++)
				{
					GLuint i0, i1, i2;
					mesh->GetTriangleVertexIndices(i, i0, i1, i2);

					auto v0 = mesh->GetVertex(i0);
					auto v1 = mesh->GetVertex(i1);
					auto v2 = mesh->GetVertex(i2);

					auto normal = glm::normalize(glm::cross(glm::normalize(v1 - v0), glm::normalize(v2 - v0)));
					vnormals[i0] += normal;
					vnormals[i1] += normal;
					vnormals[i2] += normal;
					
					vnormalRefs[i0] += 1;
					vnormalRefs[i1] += 1;
					vnormalRefs[i2] += 1;

					scene->Debug("original")->AddTriangle(v0, v1, v2);
				}

				for (size_t i = 0; i < vnormals.size(); i++)
				{
					auto v = mesh->GetVertex(i);
					auto tv = v + vnormals[i] / (float)vnormalRefs[i] * offsetDistance;
					mesh->SetVertex(i, tv);
				}
			}
			*/

			scene->GetMainCamera()->centerPosition = mesh->GetAABB().GetCenter();
			scene->GetMainCamera()->distance = mesh->GetAABB().GetDiagonalLength();

			auto shader = scene->CreateComponent<Neon::Shader>("Shader/Lighting", Neon::URL::Resource("/shader/lighting.vs"), Neon::URL::Resource("/shader/lighting.fs"));
			entity->AddComponent(shader);

			auto transform = scene->CreateComponent<Neon::Transform>("Transform/spot");
			entity->AddComponent(transform);
			entity->AddKeyEventHandler([mesh](const Neon::KeyEvent& event) {
				if (GLFW_KEY_ESCAPE == event.key && GLFW_RELEASE == event.action)
				{
					mesh->ToggleFillMode();
				}
				else if (GLFW_KEY_KP_ADD == event.key && (GLFW_PRESS == event.action || GLFW_REPEAT == event.action))
				{
					GLfloat currentPointSize;
					glGetFloatv(GL_POINT_SIZE, &currentPointSize);
					glPointSize(currentPointSize + 1.0f);
				}
				else if (GLFW_KEY_KP_SUBTRACT == event.key && (GLFW_PRESS == event.action || GLFW_REPEAT == event.action))
				{
					GLfloat currentPointSize;
					glGetFloatv(GL_POINT_SIZE, &currentPointSize);
					glPointSize(currentPointSize - 1.0f);
				}
				else if (GLFW_KEY_S == event.key && GLFW_RELEASE == event.action)
				{
					mesh->ToSTLFile("C:\\Users\\USER\\Desktop\\result.stl");
				}
				});

			entity->AddMouseButtonEventHandler([entity, scene, mesh](const Neon::MouseButtonEvent& event) {
				if (event.button == GLFW_MOUSE_BUTTON_1 && event.action == GLFW_DOUBLE_ACTION)
				{
					auto camera = scene->GetMainCamera();

					auto ray = camera->GetPickingRay(event.xpos, event.ypos);

					glm::vec3 intersection;
					size_t faceIndex = 0;
					if (mesh->Pick(ray, intersection, faceIndex))
					{
						camera->centerPosition = intersection;
					}
				}
				else if (event.button == GLFW_MOUSE_BUTTON_1 && event.action == GLFW_RELEASE)
				{
					auto camera = scene->GetMainCamera();

					auto ray = camera->GetPickingRay(event.xpos, event.ypos);

					glm::vec3 intersection;
					size_t faceIndex = 0;
					if (mesh->Pick(ray, intersection, faceIndex))
					{
						//debugPoints->Clear();

						//debugPoints->AddPoint(intersection, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

						auto vetm = entity->GetComponent<Neon::VETM>();
						auto vertex = vetm->GetNearestVertex(intersection);

						scene->Debug("Points")->Clear();
						scene->Debug("Points")->AddPoint(vertex->p, glm::red);
						scene->Debug("Points")->AddPoint(intersection, glm::white);

						scene->Debug("Lines")->Clear();
						cout << "---------------------------------------" << endl;
						for (auto& e : vertex->edges)
						{
							if (2 > e->triangles.size())
							{
								scene->Debug("Lines")->AddLine(e->v0->p, e->v1->p, glm::green, glm::green);
								cout << e->id << " : " << e->triangles.size() << endl;
							}
						}
					}
				}
				});

			

			//mesh->ToSTLFile("C:\\Resources\\3D\\STL\\Result.stl");

	/*		auto borders = vetm->GetBorderEdges();
			for (auto& borderEdges : borders)
			{
				cout << "borderEdges.size() : " << borderEdges.size() << endl;
			}*/
		}

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
