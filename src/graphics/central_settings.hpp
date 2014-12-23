#ifndef CENTRAL_SETTINGS_HPP
#define CENTRAL_SETTINGS_HPP

class CentralVideoSettings
{
private:
    /** Supports GLSL */
    bool                  m_glsl;

    int m_gl_major_version, m_gl_minor_version;
    bool hasVSLayer;
    bool hasBaseInstance;
    bool hasDrawIndirect;
    bool hasBuffserStorage;
    bool hasComputeShaders;
    bool hasTextureStorage;
    bool hasTextureView;
    bool hasBindlessTexture;
    bool m_support_sdsm;
    bool m_support_texture_compression;
    bool m_need_ubo_workaround;
    bool m_need_rh_workaround;
    bool m_need_srgb_workaround;
public:
    void init();

    // Needs special handle ?
    bool needUBOWorkaround() const;
    bool needRHWorkaround() const;
    bool needsRGBBindlessWorkaround() const;

    // Extension is available and safe to use
    bool isGLSL() const;
    unsigned getGLSLVersion() const;
    bool hasARB_base_instance() const;
    bool hasARB_draw_indirect() const;
    bool hasVSLayerExtension() const;
    bool hasBufferStorageExtension() const;
    bool hasARBComputeShaders() const;
    bool hasARBTextureStorage() const;
    bool hasARBTextureView() const;
    bool hasARBBindlessTexture() const;

    // "Macro" around feature support and user config
    bool supportsSDSM() const;
    bool supportTextureCompression() const;
    bool supportGeometryShader() const;
    bool usesShadows() const;
    bool usesGI() const;
    bool usesTextureCompression() const;
    bool useAZDO() const;
};

extern CentralVideoSettings* CVS;

#endif