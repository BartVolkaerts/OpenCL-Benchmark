#include <QLabel>
#include "raytracing.h"

Raytracing::Raytracing(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _configWidget = new QLabel("Config Widget", parent);
    _mainWidget = new RaytracingMainWidget(parent);
    _geometry = NULL;

    _imageSize.s[0] = 800;
    _imageSize.s[1] = 600;

    _cameraPlaneDistance = 150.f;

    _cameraOrigin.s[0] = 0.f;
    _cameraOrigin.s[1] = 0.f;
    _cameraOrigin.s[2] = -200.f;
    _cameraOrigin.s[3] = 0.f;

    _emisionSource.s[0] = 400.f;
    _emisionSource.s[1] = 0.f;
    _emisionSource.s[2] = 400.f;
    _emisionSource.s[3] = 0.f;

    _isRunning = false;

    connect(_mainWidget, SIGNAL(resolutionChanged(int, int)),
            this, SLOT(resolutionChanged(int, int)));

}

Raytracing::~Raytracing()
{
    delete _geometry;

}

void Raytracing::execute()
{
    _isRunning = true;
    initCL();

    renderImage();
}

void Raytracing::releaseCL()
{
    CHECK_ERR(clReleaseKernel(_createRaysKernel));
    CHECK_ERR(clReleaseKernel(_renderImageKernel));
    CHECK_ERR(clReleaseKernel(_triangleIntersectionKernel));

    releaseBuffers();
}

void Raytracing::renderImage()
{
    // Calculate worksizes:
    const size_t localWorkSize[2] = {64, 64};
    const size_t totalWorkItems[2] = {
        (_imageSize.s[0] / localWorkSize[0] + 1) * localWorkSize[0],
        (_imageSize.s[1] / localWorkSize[1] + 1) * localWorkSize[1]
    };

    // Create Camera Rays:
    CHECK_ERR(clSetKernelArg(_createRaysKernel, 0,
            sizeof(cl_mem), &_raysFromCamera));
    CHECK_ERR(clSetKernelArg(_createRaysKernel, 1,
            sizeof(cl_int2), &_imageSize));
    CHECK_ERR(clSetKernelArg(_createRaysKernel, 2,
            sizeof(cl_float), &_cameraPlaneDistance));

    CHECK_ERR(clEnqueueNDRangeKernel(
            _environment->getCommandQueue(),
            _createRaysKernel,
            2,
            0, totalWorkItems, NULL,
            0, NULL, NULL));

    // Intersect with triangle mesh:
    CHECK_ERR(clSetKernelArg(_triangleIntersectionKernel, 0,
            sizeof(cl_mem), &_raysFromCamera));
    CHECK_ERR(clSetKernelArg(_triangleIntersectionKernel, 1,
            sizeof(cl_float4), &_cameraOrigin));
    CHECK_ERR(clSetKernelArg(_triangleIntersectionKernel, 2,
            sizeof(cl_int2), &_imageSize));
    CHECK_ERR(clSetKernelArg(_triangleIntersectionKernel, 3,
            sizeof(cl_mem), &_triangleMesh));
    // Offset (argument 4) added later.
    CHECK_ERR(clSetKernelArg(_triangleIntersectionKernel, 5,
            sizeof(cl_mem), &_intersectionPoints));
    CHECK_ERR(clSetKernelArg(_triangleIntersectionKernel, 6,
            sizeof(cl_mem), &_intersectionPointNormals));
    cl_int surfaceId = 0;
    CHECK_ERR(clSetKernelArg(_triangleIntersectionKernel, 7,
            sizeof(cl_int), &surfaceId));
    CHECK_ERR(clSetKernelArg(_triangleIntersectionKernel, 8,
            sizeof(cl_mem), &_surfaceIds));

    CHECK_ERR(clEnqueueAcquireGLObjects(_environment->getCommandQueue(), 1,
                &_glTexture, 0, NULL, NULL));
    for (cl_int i = 0; i < _numberOfVertices - 2; i+=3)
    {
        CHECK_ERR(clSetKernelArg(_triangleIntersectionKernel, 4,
                sizeof(cl_int), &i));
        CHECK_ERR(clEnqueueNDRangeKernel(
                _environment->getCommandQueue(),
                _triangleIntersectionKernel,
                2,
                0, totalWorkItems, NULL,
                0, NULL, NULL));
    }
    CHECK_ERR(clEnqueueReleaseGLObjects(_environment->getCommandQueue(), 1,
                &_glTexture, 0, NULL, NULL));

    // Render image to the screen:
    CHECK_ERR(clSetKernelArg(_renderImageKernel, 0,
            sizeof(cl_mem), &_glTexture));
    CHECK_ERR(clSetKernelArg(_renderImageKernel, 1,
            sizeof(cl_int2), &_imageSize));
    CHECK_ERR(clSetKernelArg(_renderImageKernel, 2,
            sizeof(cl_mem), &_intersectionPoints));
    CHECK_ERR(clSetKernelArg(_renderImageKernel, 3,
            sizeof(cl_mem), &_intersectionPointNormals));
    CHECK_ERR(clSetKernelArg(_renderImageKernel, 4,
            sizeof(cl_float4), &_emisionSource));
    CHECK_ERR(clSetKernelArg(_renderImageKernel, 5,
            sizeof(cl_mem), &_surfaceIds));

    CHECK_ERR(clEnqueueAcquireGLObjects(_environment->getCommandQueue(), 1,
                &_glTexture, 0, NULL, NULL));
    CHECK_ERR(clEnqueueNDRangeKernel(
            _environment->getCommandQueue(),
            _renderImageKernel,
            2,
            0, totalWorkItems, NULL,
            0, NULL, NULL));
    CHECK_ERR(clEnqueueReleaseGLObjects(_environment->getCommandQueue(), 1,
                &_glTexture, 0, NULL, NULL));
    _mainWidget->updateGL();
}


void Raytracing::initCL()
{
    _environment->createGLContext();
    QStringList kernelCode;
    kernelCode.append("raytracing/create_rays.cl");
    kernelCode.append("raytracing/ray_intersection.cl");
    kernelCode.append("raytracing/render_image.cl");

    _environment->createProgram(kernelCode);

    _createRaysKernel = _environment->getKernel("createRays");
    _triangleIntersectionKernel =
        _environment->getKernel("triangleIntersection");
    _renderImageKernel = _environment->getKernel("renderImage");

    allocateBuffers();
}

void Raytracing::allocateBuffers()
{
    cl_int error;

    // Rays from camera:
    _raysFromCamera = clCreateBuffer(
            _environment->getContext(),
            CL_MEM_READ_WRITE,
            sizeof(cl_float4) * _imageSize.s[0] * _imageSize.s[1],
            NULL,
            &error);
    CHECK_ERR(error);

    // Visible image:
    _glTexture = clCreateFromGLTexture2D(
            _environment->getContext(),
            CL_MEM_READ_WRITE,
            GL_TEXTURE_2D,
            0,
            _mainWidget->getTextureId(),
            &error);
    CHECK_ERR(error);

    // Closest intersection points for every ray:
    cl_float4 *hostIntersectionPoints =
        new cl_float4[_imageSize.s[0] * _imageSize.s[1]];
    for (int i = 0; i < _imageSize.s[0] * _imageSize.s[1]; ++i)
    {
        hostIntersectionPoints[i].s[3] = -1.f;
    }
    _intersectionPoints = clCreateBuffer(
            _environment->getContext(),
            CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
            sizeof(cl_float4) * _imageSize.s[0] * _imageSize.s[1],
            hostIntersectionPoints,
            &error);
    CHECK_ERR(error);
    delete[] hostIntersectionPoints;

    // Normals for closest intersection point for every ray:
    _intersectionPointNormals = clCreateBuffer(
            _environment->getContext(),
            CL_MEM_READ_WRITE,
            sizeof(cl_float4) * _imageSize.s[0] * _imageSize.s[1],
            NULL,
            &error);
    CHECK_ERR(error);

    // Surface ids for closest intersection point for every ray:
    _surfaceIds = clCreateBuffer(
            _environment->getContext(),
            CL_MEM_READ_WRITE,
            sizeof(cl_int) * _imageSize.s[0] * _imageSize.s[1],
            NULL,
            &error);
    CHECK_ERR(error);

    // Triangle Mesh:
    createGeometry();
    _triangleMesh = clCreateBuffer(
            _environment->getContext(),
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(cl_float4) * _numberOfVertices,
            _geometry,
            &error);
    CHECK_ERR(error);


}

void Raytracing::releaseBuffers()
{
    CHECK_ERR(clReleaseMemObject(_raysFromCamera));
    CHECK_ERR(clReleaseMemObject(_glTexture));
    CHECK_ERR(clReleaseMemObject(_intersectionPoints));
    CHECK_ERR(clReleaseMemObject(_intersectionPointNormals));
    CHECK_ERR(clReleaseMemObject(_surfaceIds));
    CHECK_ERR(clReleaseMemObject(_triangleMesh));
}


void Raytracing::stop()
{
    _isRunning = false;
    releaseCL();
}

void Raytracing::createGeometry()
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile("raytracing/monkey.obj",
            aiProcess_Triangulate);// |
            //aiProcess_JoinIdenticalVertices |
            //aiProcess_SortByPType);
    const aiMesh *mesh;
    aiVector3D *vertices;


    if ( !scene )
    {
        qDebug() << "WARNING: Geometry import failed.";
        return;
    }
    if ( !scene->HasMeshes() || scene->mNumMeshes < 1 )
    {
        qDebug() << "WARNING: Can't load geometry: \
            Imported file doesn't have any meshes.";
        return;
    }
    if ( scene->mNumMeshes > 1)
    {
        qDebug() << "Imported file has multiple meshes, using first one.";
    }
    mesh = *(scene->mMeshes);
    _numberOfVertices = mesh->mNumVertices;
    vertices = mesh->mVertices;

    _geometry = new cl_float4[_numberOfVertices];

    for ( int i = 0; i < _numberOfVertices; ++i )
    {
        _geometry[i].s[0] = vertices[i].x * 50;
        _geometry[i].s[1] = vertices[i].y * 50;
        _geometry[i].s[2] = vertices[i].z * 50 - 70;
        qDebug() << "x:" << _geometry[i].s[0] << "y:" << _geometry[i].s[1] << "z:" << _geometry[i].s[2];
    }
}

void Raytracing::resolutionChanged(int width, int height)
{
    _imageSize.s[0] = width;
    _imageSize.s[1] = height;
    if (_isRunning)
    {
        releaseBuffers();
        allocateBuffers();
    }
}

QWidget *Raytracing::getConfigWidget()
{
    return _configWidget;
}

QWidget *Raytracing::getMainWidget()
{
    return _mainWidget;
}
