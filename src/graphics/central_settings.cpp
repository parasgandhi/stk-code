#include "central_settings.hpp"
#include "modes/profile_world.hpp"
#include "gl_headers.hpp"
#include "glwrap.hpp"
#include "graphics_restrictions.hpp"

CentralVideoSettings *CVS = new CentralVideoSettings();

void CentralVideoSettings::init()
{
    m_gl_major_version = 2;
    m_gl_minor_version = 1;
    // Call to glGetIntegerv should not be made if --no-graphics is used
    if (!ProfileWorld::isNoGraphics())
    {

    }
    if (!ProfileWorld::isNoGraphics())
    {
        glGetIntegerv(GL_MAJOR_VERSION, &m_gl_major_version);
        glGetIntegerv(GL_MINOR_VERSION, &m_gl_minor_version);
        Log::info("IrrDriver", "OpenGL version: %d.%d", m_gl_major_version, m_gl_minor_version);
        Log::info("IrrDriver", "OpenGL vendor: %s", glGetString(GL_VENDOR));
        Log::info("IrrDriver", "OpenGL renderer: %s", glGetString(GL_RENDERER));
        Log::info("IrrDriver", "OpenGL version string: %s", glGetString(GL_VERSION));

        m_need_ubo_workaround = false;
        m_need_rh_workaround = false;
        m_need_srgb_workaround = false;
        m_support_sdsm = true;
        m_support_texture_compression = true;
        if (strstr((const char *)glGetString(GL_VENDOR), "Intel") != NULL)
        {
            // Intel on windows doesnt support srgb compressed textures properly
            m_support_texture_compression = false;
#ifdef WIN32
            // Fix for Intel Sandy Bridge on Windows which supports GL up to 3.1 only
            if (m_gl_major_version == 3 && m_gl_minor_version == 1)
                m_need_ubo_workaround = true;
#endif
        }

        // Fix for Nvidia and instanced RH
        if (strstr((const char *)glGetString(GL_VENDOR), "NVIDIA") != NULL)
        {
            m_need_rh_workaround = true;
            m_support_sdsm = false;
        }

        // Fix for AMD and bindless sRGB textures
        if (strstr((const char *)glGetString(GL_VENDOR), "ATI") != NULL)
            m_need_srgb_workaround = true;
    }
#ifdef WIN32
    m_glsl = (m_gl_major_version > 3 || (m_gl_major_version == 3 && m_gl_minor_version >= 1));
#else
    m_glsl = (m_gl_major_version > 3 || (m_gl_major_version == 3 && m_gl_minor_version >= 1));
#endif
    if (!ProfileWorld::isNoGraphics())
        initGL();

    // Parse extensions
    hasVSLayer = false;
    hasBaseInstance = false;
    hasBuffserStorage = false;
    hasDrawIndirect = false;
    hasComputeShaders = false;
    hasTextureStorage = false;
    hasTextureView = false;
    hasBindlessTexture = false;

    // Default false value for hasVSLayer if --no-graphics argument is used
#if !defined(__APPLE__)
    if (!ProfileWorld::isNoGraphics())
    {
        if (hasGLExtension("GL_AMD_vertex_shader_layer")) {
            hasVSLayer = true;
            Log::info("GLDriver", "AMD Vertex Shader Layer enabled");
        }
        if (hasGLExtension("GL_ARB_buffer_storage")) {
            hasBuffserStorage = true;
            Log::info("GLDriver", "ARB Buffer Storage enabled");
        }
        if (hasGLExtension("GL_ARB_base_instance")) {
            hasBaseInstance = true;
            Log::info("GLDriver", "ARB Base Instance enabled");
        }
        if (hasGLExtension("GL_ARB_draw_indirect")) {
            hasDrawIndirect = true;
            Log::info("GLDriver", "ARB Draw Indirect enabled");
        }
        if (hasGLExtension("GL_ARB_compute_shader")) {
            hasComputeShaders = true;
            Log::info("GLDriver", "ARB Compute Shader enabled");
        }
        if (hasGLExtension("GL_ARB_texture_storage")) {
            hasTextureStorage = true;
            Log::info("GLDriver", "ARB Texture Storage enabled");
        }
        if (hasGLExtension("GL_ARB_texture_view")) {
            hasTextureView = true;
            Log::info("GLDriver", "ARB Texture View enabled");
        }
        if (hasGLExtension("GL_ARB_bindless_texture")) {
            hasBindlessTexture = true;
            Log::info("GLDriver", "ARB Bindless Texture enabled");
        }
        m_support_sdsm = m_support_sdsm && hasComputeShaders && hasBuffserStorage;

        std::string driver((char*)(glGetString(GL_VERSION)));
        std::string card((char*)(glGetString(GL_RENDERER)));
        std::vector<std::string> restrictions =
            GraphicsRestrictions::getRestrictions(driver, card);

        for (const std::string &restriction : restrictions)
        {
            if (!restriction.compare("BufferStorage"))
            {
                hasBuffserStorage = false;
                Log::info("Graphics restrictions", "Buffer Storage disabled");
            }
        }
    }
#else
    m_support_sdsm = false;
#endif
}

unsigned CentralVideoSettings::getGLSLVersion() const
{
    if (m_gl_major_version > 3 || (m_gl_major_version == 3 && m_gl_minor_version == 3))
        return m_gl_major_version * 100 + m_gl_minor_version * 10;
    else if (m_gl_major_version == 3)
        return 100 + (m_gl_minor_version + 3) * 10;
    else
        return 120;
}

bool CentralVideoSettings::supportsSDSM() const
{
    return m_support_sdsm && UserConfigParams::m_sdsm;
}

bool CentralVideoSettings::supportTextureCompression() const
{
    return m_support_texture_compression;
}

bool CentralVideoSettings::supportGeometryShader() const
{
    return getGLSLVersion() >= 330;
}

bool CentralVideoSettings::usesShadows() const
{
    return supportGeometryShader() && UserConfigParams::m_shadows && !needUBOWorkaround();
}

bool CentralVideoSettings::usesGI() const
{
    return supportGeometryShader() && UserConfigParams::m_gi && !needUBOWorkaround();
}

bool CentralVideoSettings::usesTextureCompression() const
{
    return UserConfigParams::m_texture_compression && m_support_texture_compression;
}

bool CentralVideoSettings::useAZDO() const
{
    return hasBindlessTexture && UserConfigParams::m_azdo;
}

bool CentralVideoSettings::needUBOWorkaround() const
{
    return m_need_ubo_workaround;
}

bool CentralVideoSettings::needRHWorkaround() const
{
    return m_need_rh_workaround;
}

bool CentralVideoSettings::needsRGBBindlessWorkaround() const
{
    return m_need_srgb_workaround;
}

bool CentralVideoSettings::hasARB_base_instance() const
{
    return hasBaseInstance;
}

bool CentralVideoSettings::hasARB_draw_indirect() const
{
    return hasDrawIndirect;
}

bool CentralVideoSettings::hasVSLayerExtension() const
{
    return hasVSLayer;
}

bool CentralVideoSettings::hasBufferStorageExtension() const
{
    return hasBuffserStorage;
}

bool CentralVideoSettings::hasARBComputeShaders() const
{
    return hasComputeShaders;
}

bool CentralVideoSettings::hasARBTextureStorage() const
{
    return hasTextureStorage;
}

bool CentralVideoSettings::hasARBTextureView() const
{
    return hasTextureView;
}

bool CentralVideoSettings::hasARBBindlessTexture() const
{
    return hasBindlessTexture;
}

bool CentralVideoSettings::isGLSL() const
{
    return m_glsl;
}