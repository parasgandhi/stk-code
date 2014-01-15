#include "stkmesh.hpp"
#include "graphics/irr_driver.hpp"
#include <ISceneManager.h>
#include <IMaterialRenderer.h>

namespace ObjectRefShader
{
	GLuint Program;
	GLuint attrib_position, attrib_texcoord, attrib_normal;
	GLuint uniform_MVP, uniform_TIMV, uniform_texture;

	void init()
	{
		initGL();
		Program = LoadProgram(file_manager->getAsset("shaders/object.vert").c_str(), file_manager->getAsset("shaders/objectref.frag").c_str());
		attrib_position = glGetAttribLocation(Program, "Position");
		attrib_texcoord = glGetAttribLocation(Program, "Texcoord");
		attrib_normal = glGetAttribLocation(Program, "Normal");
		uniform_MVP = glGetUniformLocation(Program, "ModelViewProjectionMatrix");
		uniform_TIMV = glGetUniformLocation(Program, "TransposeInverseModelView");
		uniform_texture = glGetUniformLocation(Program, "texture");
	}

	GLuint createVAO(GLuint vbo, GLuint idx, size_t stride)
	{
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(attrib_position);
		glEnableVertexAttribArray(attrib_texcoord);
		glEnableVertexAttribArray(attrib_normal);
		glVertexAttribPointer(attrib_position, 3, GL_FLOAT, GL_FALSE, stride, 0);
		glVertexAttribPointer(attrib_texcoord, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*) 28);
		glVertexAttribPointer(attrib_normal, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*) 12);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx);
		glBindVertexArray(0);
		return vao;
	}

	void setProgramAndUniforms()
	{
		glUseProgram(Program);
		core::matrix4 ModelViewProjectionMatrix = irr_driver->getVideoDriver()->getTransform(video::ETS_PROJECTION);
		ModelViewProjectionMatrix *= irr_driver->getVideoDriver()->getTransform(video::ETS_VIEW);
		ModelViewProjectionMatrix *= irr_driver->getVideoDriver()->getTransform(video::ETS_WORLD);
		core::matrix4 TransposeInverseModelView = irr_driver->getVideoDriver()->getTransform(video::ETS_VIEW);
		TransposeInverseModelView *= irr_driver->getVideoDriver()->getTransform(video::ETS_WORLD);
		TransposeInverseModelView.makeInverse();
		TransposeInverseModelView = TransposeInverseModelView.getTransposed();

		glUniformMatrix4fv(uniform_MVP, 1, GL_FALSE, ModelViewProjectionMatrix.pointer());
		glUniformMatrix4fv(uniform_TIMV, 1, GL_FALSE, TransposeInverseModelView.pointer());
	}
}

namespace ObjectShader
{
	GLuint Program;
	GLuint attrib_position, attrib_texcoord, attrib_normal;
	GLuint uniform_MVP, uniform_TIMV, uniform_texture;

	void init()
	{
		initGL();
		Program = LoadProgram(file_manager->getAsset("shaders/object.vert").c_str(), file_manager->getAsset("shaders/object.frag").c_str());
		attrib_position = glGetAttribLocation(Program, "Position");
		attrib_texcoord = glGetAttribLocation(Program, "Texcoord");
		attrib_normal = glGetAttribLocation(Program, "Normal");
		uniform_MVP = glGetUniformLocation(Program, "ModelViewProjectionMatrix");
		uniform_TIMV = glGetUniformLocation(Program, "TransposeInverseModelView");
		uniform_texture = glGetUniformLocation(Program, "texture");
	}

	GLuint createVAO(GLuint vbo, GLuint idx, size_t stride)
	{
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(attrib_position);
		glEnableVertexAttribArray(attrib_texcoord);
		glEnableVertexAttribArray(attrib_normal);
		glVertexAttribPointer(attrib_position, 3, GL_FLOAT, GL_FALSE, stride, 0);
		glVertexAttribPointer(attrib_texcoord, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*) 28);
		glVertexAttribPointer(attrib_normal, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*) 12);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx);
		glBindVertexArray(0);
		return vao;
	}

	void setProgramAndUniforms()
	{
		glUseProgram(Program);
		core::matrix4 ModelViewProjectionMatrix = irr_driver->getVideoDriver()->getTransform(video::ETS_PROJECTION);
		ModelViewProjectionMatrix *= irr_driver->getVideoDriver()->getTransform(video::ETS_VIEW);
		ModelViewProjectionMatrix *= irr_driver->getVideoDriver()->getTransform(video::ETS_WORLD);
		core::matrix4 TransposeInverseModelView = irr_driver->getVideoDriver()->getTransform(video::ETS_VIEW);
		TransposeInverseModelView *= irr_driver->getVideoDriver()->getTransform(video::ETS_WORLD);
		TransposeInverseModelView.makeInverse();
		TransposeInverseModelView = TransposeInverseModelView.getTransposed();

		glUniformMatrix4fv(uniform_MVP, 1, GL_FALSE, ModelViewProjectionMatrix.pointer());
		glUniformMatrix4fv(uniform_TIMV, 1, GL_FALSE, TransposeInverseModelView.pointer());
	}
}

static
GLMesh allocateMeshBuffer(scene::IMeshBuffer* mb)
{
	initGL();
	GLMesh result;
	if (!mb)
		return result;
	glGenBuffers(1, &(result.vertex_buffer));
	glGenBuffers(1, &(result.index_buffer));

	glBindBuffer(GL_ARRAY_BUFFER, result.vertex_buffer);
	const void* vertices=mb->getVertices();
	const u32 vertexCount=mb->getVertexCount();
	const irr::video::E_VERTEX_TYPE vType=mb->getVertexType();
	const u32 vertexSize = getVertexPitchFromType(vType);
	const c8* vbuf = static_cast<const c8*>(vertices);
	core::array<c8> buffer;
	buffer.set_used(vertexSize * vertexCount);
	memcpy(buffer.pointer(), vertices, vertexSize * vertexCount);
	vbuf = buffer.const_pointer();
	glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, vbuf, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.index_buffer);
	const void* indices=mb->getIndices();
	u32 indexCount= mb->getIndexCount();
	GLenum indexSize;
	switch (mb->getIndexType())
	{
		case irr::video::EIT_16BIT:
		{
			indexSize=sizeof(u16);
			break;
		}
		case irr::video::EIT_32BIT:
		{
			indexSize=sizeof(u32);
			break;
		}
		default:
		{
			assert(0 && "Wrong index size");
		}
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	result.IndexCount = mb->getIndexCount();
	switch (mb->getPrimitiveType())
	{
	case scene::EPT_POINTS:
		result.PrimitiveType = GL_POINTS;
		break;
	case scene::EPT_TRIANGLE_STRIP:
		result.PrimitiveType = GL_TRIANGLE_STRIP;
		break;
	case scene::EPT_TRIANGLE_FAN:
		result.PrimitiveType = GL_TRIANGLE_FAN;
		break;
	case scene::EPT_LINES:
		result.PrimitiveType = GL_LINES;
	case scene::EPT_TRIANGLES:
		result.PrimitiveType = GL_TRIANGLES;
		break;
	case scene::EPT_POINT_SPRITES:
	case scene::EPT_LINE_LOOP:
	case scene::EPT_POLYGON:
	case scene::EPT_LINE_STRIP:
	case scene::EPT_QUAD_STRIP:
	case scene::EPT_QUADS:
		assert(0 && "Unsupported primitive type");
	}
	switch (mb->getVertexType())
	{
	case video::EVT_STANDARD:
		result.Stride = sizeof(video::S3DVertex);
		break;
	case video::EVT_2TCOORDS:
		result.Stride = sizeof(video::S3DVertex2TCoords);
		break;
	case video::EVT_TANGENTS:
		result.Stride = sizeof(video::S3DVertexTangents);
		break;
	}
	switch (mb->getIndexType())
	{
	case video::EIT_16BIT:
		result.IndexType = GL_UNSIGNED_SHORT;
		break;
	case video::EIT_32BIT:
		result.IndexType = GL_UNSIGNED_INT;
		break;
	}
	ITexture *tex = mb->getMaterial().getTexture(0);
	if (tex)
		result.textures = static_cast<irr::video::COpenGLTexture*>(tex)->getOpenGLTextureName();
	else
		result.textures = 0;
	result.vao = 0;
	return result;
}

STKMesh::STKMesh(irr::scene::IMesh* mesh, ISceneNode* parent, irr::scene::ISceneManager* mgr,	irr::s32 id,
	const irr::core::vector3df& position,
	const irr::core::vector3df& rotation,
	const irr::core::vector3df& scale) :
		CMeshSceneNode(mesh, parent, mgr, id, position, rotation, scale)
{
	for (u32 i=0; i<Mesh->getMeshBufferCount(); ++i)
	{
		scene::IMeshBuffer* mb = Mesh->getMeshBuffer(i);
		GLmeshes.push_back(allocateMeshBuffer(mb));

	}
	if (ObjectRefShader::Program)
		return;
	ObjectRefShader::init();
	ObjectShader::init();
}

STKMesh::~STKMesh()
{
//	glDeleteBuffers(vertex_buffer.size(), vertex_buffer.data());
//	glDeleteBuffers(index_buffer.size(), index_buffer.data());
}

static
void draw(const GLMesh &mesh, video::E_MATERIAL_TYPE type)
{
	if (!mesh.textures)
		return;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	GLenum ptype = mesh.PrimitiveType;
	GLenum itype = mesh.IndexType;
	size_t count = mesh.IndexCount;

	if (type == irr_driver->getShader(ES_OBJECTPASS_REF))
	{
		ObjectRefShader::setProgramAndUniforms();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh.textures);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glUniform1f(ObjectRefShader::uniform_texture, 0);
	}
	if (type == irr_driver->getShader(ES_OBJECTPASS))
	{
		ObjectShader::setProgramAndUniforms();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh.textures);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glUniform1f(ObjectShader::uniform_texture, 0);
	}

	glBindVertexArray(mesh.vao);
	glDrawElements(ptype, count, itype, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	video::SMaterial material;
	material.MaterialType = irr_driver->getShader(ES_RAIN);
	material.BlendOperation = video::EBO_NONE;
	material.ZWriteEnable = true;
	material.Lighting = false;
	irr_driver->getVideoDriver()->setMaterial(material);
	static_cast<irr::video::COpenGLDriver*>(irr_driver->getVideoDriver())->setRenderStates3DMode();
}

static bool isObject(video::E_MATERIAL_TYPE type)
{
	if (type == irr_driver->getShader(ES_OBJECTPASS_REF) || type == irr_driver->getShader(ES_OBJECTPASS))
		return true;
	return false;
}

static void initvaostate(GLMesh &mesh, video::E_MATERIAL_TYPE type)
{
	if (mesh.vao)
		return;
	if (type == irr_driver->getShader(ES_OBJECTPASS_REF))
		mesh.vao = ObjectRefShader::createVAO(mesh.vertex_buffer, mesh.index_buffer, mesh.Stride);
	if (type == irr_driver->getShader(ES_OBJECTPASS))
		mesh.vao = ObjectShader::createVAO(mesh.vertex_buffer, mesh.index_buffer, mesh.Stride);
}

void STKMesh::render()
{
	irr::video::IVideoDriver* driver = irr_driver->getVideoDriver();

	if (!Mesh || !driver)
		return;

	bool isTransparentPass =
		SceneManager->getSceneNodeRenderPass() == scene::ESNRP_TRANSPARENT;

	++PassCount;

	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
	Box = Mesh->getBoundingBox();

	for (u32 i=0; i<Mesh->getMeshBufferCount(); ++i)
	{
		scene::IMeshBuffer* mb = Mesh->getMeshBuffer(i);
		if (mb)
		{
			const video::SMaterial& material = ReadOnlyMaterials ? mb->getMaterial() : Materials[i];

			video::IMaterialRenderer* rnd = driver->getMaterialRenderer(material.MaterialType);
			bool transparent = (rnd && rnd->isTransparent());

#ifdef OGL32CTX
			// only render transparent buffer if this is the transparent render pass
			// and solid only in solid pass
			if (isObject(material.MaterialType) && !isTransparentPass && !transparent)
			{
				initvaostate(GLmeshes[i], material.MaterialType);
				draw(GLmeshes[i], material.MaterialType);
			}
			else if (transparent == isTransparentPass)
#else
			if (transparent == isTransparentPass)
#endif
			{
				driver->setMaterial(material);
				driver->drawMeshBuffer(mb);
			}
		}
	}
}