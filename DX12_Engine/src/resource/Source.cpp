/// GENERATE PRIMITIVE MESH HERE :
DX12Mesh * DX12Mesh::GeneratePrimitiveMesh(EPrimitiveMesh i_Prim)
{
	DX12Mesh * returnMesh = nullptr;
	// default primitive mesh flags
	static const UINT64 flags =
		DX12PipelineState::EElementFlags::eHaveNormal |
		DX12PipelineState::EElementFlags::eHaveTexcoord;

	switch (i_Prim)
	{
	case ePlane:
		returnMesh = new DX12Mesh(s_PrimitiveLayoutDesc,
			reinterpret_cast<BYTE*>(vPlane), 4u, iPlane, 6u);
		returnMesh->m_RootMeshBuffer->m_Name = L"Plane";
		break;
	case eTriangle:
		returnMesh = new DX12Mesh(s_PrimitiveLayoutDesc,
			reinterpret_cast<BYTE*>(vTriangle), 3u);
		returnMesh->m_RootMeshBuffer->m_Name = L"Triangle";
		break;
	case eCube:
		returnMesh = new DX12Mesh(s_PrimitiveLayoutDesc,
			reinterpret_cast<BYTE*>(vCube), 24u, iCube, 36u);
		returnMesh->m_RootMeshBuffer->m_Name = L"Cube";
		break;
	}



// helpers
inline DX12Texture * LoadTexture(const std::string & i_TexName, const std::string & i_Folder, ResourcesManager * i_Manager)
{
	if (i_TexName == "")
		return nullptr;

	const std::string filepath = i_Folder + i_TexName;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> stringConverter;
	const std::wstring wFilepath = stringConverter.from_bytes(filepath.c_str());

	DX12Texture * texture = i_Manager->GetTexture(wFilepath.c_str());
	if (texture != nullptr && texture->IsLoaded())
	{
		return texture;
	}

	PRINT_DEBUG("Error when loading %s", i_TexName.c_str());
	DEBUG_BREAK;
	return nullptr;
}

DX12Mesh * DX12Mesh::LoadMeshObj(const char * i_Filename, const char * i_MaterialFolder, const char * i_TextureFolder)
{
	ResourcesManager * resourcesManager = Engine::GetInstance().GetResourcesManager();

	tinyobj::attrib_t					attrib;
	std::vector<tinyobj::shape_t>		shapes;
	std::vector<tinyobj::material_t>	material;

	std::string error;
	Clock timer;
	timer.Restart();

	// create load directory
	std::string textureFolder;
	std::string materialFolder;
	std::string objFilepath(i_Filename);

	if (i_TextureFolder == nullptr)
	{
		if (i_MaterialFolder != nullptr)
		{
			textureFolder.append(i_MaterialFolder);
		}
		else
		{
			// default texture folder
			textureFolder.append("resources/tex/");
		}
	}

	if (i_MaterialFolder == nullptr)
	{
		// retreive the same folder as the obj
		size_t i = objFilepath.find_last_of("/");

		if (i != std::string::npos)
		{
			materialFolder.append(objFilepath, 0, (i + 1));
		}
		else
		{
			// default material folder
			materialFolder.append("resources/mat/");
		}
	}

	// load the model
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &material, &error, objFilepath.c_str(), materialFolder.c_str());
	float loadTime = timer.GetElaspedTime().ToSeconds();

#ifdef _DEBUG
	// display debug message
	std::replace(error.begin(), error.end(), '\n', ' ');
	std::ostringstream stream;
	stream << "load obj : " << i_Filename << " : load time (" << loadTime << " sec)"
		<< std::endl << (ret ? "[No Error]" : error);

	std::string message(stream.str());
	PRINT_DEBUG(message.c_str());
#endif

	if (!ret)
	{
		DEBUG_BREAK;
		return nullptr;
	}

	// load mesh information and create the stream
	DX12Mesh * mesh = new DX12Mesh;

	for (size_t sh = 0; sh < shapes.size(); ++sh)
	{
		// create buffer and initialize data
		UINT stride = 3;	// default stride in float (3 float for positions)
		tinyobj::shape_t * shape = &shapes[sh];
		const size_t verticeCount = shape->mesh.indices.size();

		// compute the flag :
		// by default the mesh always have normals
		const tinyobj::index_t origin = shape->mesh.indices[0];
		UINT64 flags = DX12PipelineState::EElementFlags::eNone;

		// compute stride and flags for the mesh
		if (origin.normal_index != -1)
		{
			flags |= DX12PipelineState::EElementFlags::eHaveNormal;
			stride += 3;
		}
		if (origin.texcoord_index != -1)
		{
			flags |= DX12PipelineState::EElementFlags::eHaveTexcoord;
			stride += 2;
		}
		// do not support vertex color for now
		//if (attrib.colors.size() != 0 && (attrib.colors[0] != -1.0f) /* verify that color is really present in the model */)
		//{
		//	flags |= DX12PipelineState::EElementFlags::eHaveColor;
		//	stride += 3;
		//}

		// only one supported type for now
		ASSERT(flags & DX12PipelineState::EElementFlags::eHaveNormal && flags & DX12PipelineState::EElementFlags::eHaveTexcoord);

		FLOAT * const verticeBuffer = new FLOAT[verticeCount * stride];
		FLOAT * bufferItr = verticeBuffer;

		for (size_t id = 0; id < shape->mesh.indices.size(); ++id)
		{
			const tinyobj::index_t index = shape->mesh.indices[id];

			// layout order definition depending flags : 
			// 1 - Position
			// 2 - Normal
			// 3 - Texcoord
			// 4 - Color

			// copy the position to the buffer
			memcpy(bufferItr, &attrib.vertices[3 * index.vertex_index], 3 * sizeof(FLOAT));
			bufferItr += 3;
			if (flags & DX12PipelineState::EElementFlags::eHaveNormal)
			{
				// copy the normal
				memcpy(bufferItr, &attrib.normals[3 * index.normal_index], 3 * sizeof(FLOAT));
				bufferItr += 3;
			}
			if (flags & DX12PipelineState::EElementFlags::eHaveTexcoord)
			{
				// copy the uv
				memcpy(bufferItr, &attrib.texcoords[2 * index.texcoord_index], 2 * sizeof(FLOAT));
				bufferItr += 2;
			}
			//if (flags & DX12Mesh::EElementFlags::eHaveColor)
			//{
			//	// copy the color
			//	memcpy(bufferItr, &attrib.colors[3 * index.vertex_index], 3 * sizeof(FLOAT));
			//	bufferItr += 3;
			//}
		}

		// Get the name of the shape
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> stringConverter;
		std::wstring wname = stringConverter.from_bytes(shape->name);

		D3D12_INPUT_LAYOUT_DESC layout;
		DX12PipelineState::CreateInputLayoutFromFlags(layout, flags);

		DX12MeshBuffer * meshBuffer = new DX12MeshBuffer(
			layout,	// the generated layout depending on the flags
			reinterpret_cast<BYTE*>(verticeBuffer),
			(UINT)verticeCount,
			wname.c_str());

		// retreive materials of the mesh
		std::vector<int> meshMaterials(shape->mesh.material_ids);
		meshMaterials.erase(std::unique(meshMaterials.begin(), meshMaterials.end()), meshMaterials.end());

		if (meshMaterials.size() > 1)
		{
			// To do : create other mesh with multiple material
		}

		// load materials
		for (size_t i = 0; i < meshMaterials.size(); ++i)
		{
			tinyobj::material_t mat = material[meshMaterials[i]];

			DX12Material::DX12MaterialDesc desc;

			// setup the name of the material
			desc.Name = mat.name;

			// load textures
			desc.map_Ka = LoadTexture(mat.ambient_texname, textureFolder, resourcesManager);
			desc.map_Kd = LoadTexture(mat.diffuse_texname, textureFolder, resourcesManager);
			desc.map_Ks = LoadTexture(mat.specular_texname, textureFolder, resourcesManager);
			// to do : load other textures

			// retreive other data
			desc.Ka = mat.ambient;
			desc.Kd = mat.diffuse;
			desc.Ke = mat.emission;
			desc.Ks = mat.specular;

			meshBuffer->SetDefaultMaterial(desc);
		}

		// create mesh and initialize it
		mesh->m_SubMeshBuffer.push_back(meshBuffer);

		// cleanup the resources
		delete[] verticeBuffer;
	}

	if (shapes.size() == 1)
	{
		// we put the only one submesh from the submeshes to the root mesh
		mesh->m_RootMeshBuffer = mesh->m_SubMeshBuffer[0];
		mesh->m_SubMeshBuffer.clear();
	}

	return mesh;
}