var scene;
var camera;
var renderer;
var group;



function updateModel(glUpdateData) {
    $.getJSON(glUpdateData.dataUrl, function(data) {
        console.log("updating gl");

        var quaternion = new THREE.Quaternion()
        quaternion.fromArray(data[0].data);
        quaternion.normalize();

        group.setRotationFromQuaternion(quaternion)
        render();
    });

    glUpdateData.timeoutContext.setTimeout(
        function() {
             updateModel(modelUpdateData);
        },
        updatePeriodGLMs
    );
}

function mccGLMain(container, modelUrl, dataUrl)
{
    width = container.width();
    height = container.height();


    // Scene
    scene = new THREE.Scene();
    camera = new THREE.PerspectiveCamera( 75, width / height, 0.1, 1000 );
    camera.position.set(30, 30, 30);
    var cameraTarget = new THREE.Vector3(0, 0, 0);
    camera.lookAt(cameraTarget);

    // Plane
    var plane = new THREE.Mesh(
    new THREE.PlaneBufferGeometry( 40, 40 ),
    new THREE.MeshPhongMaterial( { color: 0x999999, specular: 0x101010 } )
    );
    plane.rotation.x = -Math.PI/2;
    plane.position.y = -0.5;
    scene.add( plane );

    plane.receiveShadow = true;

    // Lights
    scene.add( new THREE.HemisphereLight( 0x443333, 0x111122 ) );

    // addShadowedLight( 1, 1, 1, 0xffffff, 1.35 );
    // addShadowedLight( 0.5, 1, -1, 0xffaa00, 1 );

    // Rocket
    var loader = new THREE.STLLoader();
    group = new THREE.Object3D();
    loader.load(modelUrl, function (geometry) {
        console.log(geometry);
        var mat = new THREE.MeshLambertMaterial({color: 0x7777ff});
        group = new THREE.Mesh(geometry, mat);
        group.translateY(15);
        debugger;
        group.rotation.x = -0.5 * Math.PI;
        group.rotation.z = -0.5 * Math.PI;
        group.scale.set(0.2, 0.2, 0.2);
        scene.add(group);
    });


    // renderer
    renderer = new THREE.WebGLRenderer();
    renderer.setPixelRatio( window.devicePixelRatio );
    renderer.gammaInput = true;
    renderer.gammaOutput = true;
    renderer.shadowMap.enabled = true;
    renderer.setSize(width, height);
    container.append(renderer.domElement);


    render();

    modelUpdateData = {
        dataUrl: dataUrl,
        timeoutContext: this
    };

    setTimeout(
        function() {
             updateModel(modelUpdateData);
        },
        updatePeriodGLMs
    );
}


function render(){
    requestAnimationFrame(render);
    renderer.render(scene, camera);
}