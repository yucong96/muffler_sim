
padding = 10;

window_width = window.innerWidth;
window_height = window.innerHeight;

document.getElementById('whole-container').style.width = window_width.toString() + 'px';
document.getElementById('whole-container').style.height = window_height.toString() + 'px';

width = 3 / 4 * window_width;
height = 5 / 8 * window_height;
console.log(window_width);
console.log(width);

document.getElementById('canvas-container').style.width = width.toString() + 'px';
document.getElementById('canvas-container').style.height = height.toString() + 'px';
document.getElementById('control-container').style.paddingRight = padding;
document.getElementById('control-container').style.paddingLeft = padding;
document.getElementById('control-container').style.width = (window_width - width - 2*padding).toString() + 'px';
document.getElementById('control-container').style.height = window_height.toString() + 'px';



var chamber_data = [];
var default_chamber_data = {
    'radius': 0.05,
    'length': 0.1
};
function get_default_copy() {
    var data = {};
    data['radius'] = default_chamber_data['radius'];
    data['length'] = default_chamber_data['length'];
    return data;
}
function init_chamber_data() {
    chamber_data.push(get_default_copy());
    update_chamber_data();
}
function update_chamber_data() {
    $(document).ready(function() {
	var html = '';
	for (var i=1; i<=chamber_data.length; i++) {
	    var chamber_title_html = '' +
		'  <div class="row">' +
		'    <div class="col-sm-6">' +
		'      <h3>Chamber' + i.toString() + '</h3>' +
		'    </div>';
	    if (chamber_data.length != 3) {
		chamber_title_html += '' +
		    '<div class="col-sm-2" style="margin-top:17px;">' +
		    '  <button id="chamber-btn-insert-' + i.toString() + '" class="btn btn-primary chamber-ctrl-btn" type="button">INSERT</button>' +
		    '</div>';
	    }
	    if (chamber_data.length != 1) {
		chamber_title_html += '' +
		    '<div class="col-sm-3" style="margin-top:17px;">' +
		    '  <button id="chamber-btn-remove-' + i.toString() + '" class="btn btn-primary chamber-ctrl-btn" type="button">REMOVE</button>' +
		    '</div>';
	    }
	    chamber_title_html += '' +
		'  </div>';
	    var chamber_radius_html = '' +
		'<div class="row">' +
		'  <div class="col-sm-3 col-sm-offset-1">' +
		'    <h4>Radius</h4>' +
		'  </div>' +
		'  <div class="col-sm-8" style="margin-top:13px">' +
		'    <input id="slider-radius-' + i.toString() + '" data-slider-id="slider-radius-' + i.toString() + '" type="text" data-slider-min="0.01" data-slider-max="0.3" data-slider-step="0.01" data-slider-value="' + chamber_data[i-1]['radius'] + '"/>' +
		'  </div>' +
		'</div>';
	    var chamber_length_html = '' +
		'<div class="row">' +
		'  <div class="col-sm-3 col-sm-offset-1">' +
		'    <h4>Length</h4>' +
		'  </div>' +
		'  <div class="col-sm-8" style="margin-top:13px">' +
		'    <input id="slider-length-' + i.toString() + '" data-slider-id="slider-length-' + i.toString() + '" type="text" data-slider-min="0.01" data-slider-max="0.5" data-slider-step="0.01" data-slider-value="' + chamber_data[i-1]['length'] + '"/>' +
		'  </div>' +
		'</div>';
	    html += chamber_title_html + chamber_radius_html + chamber_length_html;
	}
	$('#chamber-ctrl-panel').html(html);

	for (var i=1; i<=chamber_data.length; i++) {
	    $('#slider-radius-' + i.toString()).slider({
		formatter: function(value) {
		    var id = $(this).attr('id');
		    var strs = id.split('-');
		    var index = strs[strs.length-1];
		    chamber_data[index-1]['radius'] = value;
		    return value;
		}
	    });
	    $('#slider-length-' + i.toString()).slider({
		formatter: function(value) {
		    var id = $(this).attr('id');
		    var strs = id.split('-');
		    var index = strs[strs.length-1];
		    chamber_data[index-1]['length'] = value;
		    return value;
		}
	    });
	}

	init_chamber_ctrl_click();
    }); 
}
function init_chamber_ctrl_click() {
    $(document).ready(function() {
	$('.chamber-ctrl-btn').click(function() {
	    var id = $(this).attr('id');
	    var strs = id.split('-');
	    var index = strs[strs.length-1];
	    var type = strs[strs.length-2];
	    if (type == 'insert') {
		chamber_data.splice(index, 0, get_default_copy());
		update_chamber_data();
	    } else if (type == 'remove') {
		chamber_data.splice(index-1, 1);
		update_chamber_data();
	    } else {
		alert('There is a bug');
	    }
	});
    });
}

















var scene;
function init_scene() {
    scene = new THREE.Scene();
}


var camera;
function init_camera() {
    camera = new THREE.PerspectiveCamera(75, width / height, 0.01, 10);
    camera.position.z = 0.2;
    camera.lookAt(new THREE.Vector3(0, 0, 0));
    scene.add(camera);
}


var renderer;
function init_renderer() {
    renderer = new THREE.WebGLRenderer();
    renderer.setSize(width, height);
    document.getElementById('canvas').appendChild(renderer.domElement);
}


var light;
function init_light() {
    light = new THREE.DirectionalLight(0xffffff);
    light.position.set(200, 200, 1000).normalize();
    camera.add(light);
    camera.add(light.target);
}


var material;
var loader = new THREE.VTKLoader();
function init_model() {
    material = new THREE.MeshLambertMaterial({color : 0xffffff, side : THREE.DoubleSide});
    loader.load("static/model/surface.vtk", function(geometry) {
	geometry.center();
	geometry.computeVertexNormals();

	var mesh = new THREE.Mesh(geometry, material);
	mesh.position.set(0, 0, 0);
	mesh.scale.multiplyScalar(0.4);
	scene.add(mesh);

    });
}


var controls;
function init_controls() {
    controls = new THREE.TrackballControls(camera);
    controls.rotateSpeed = 5;
    controls.zoomSpeed = 3;
    controls.panSpeed = 0.8;
    controls.staticMoving = true;
    controls.dynamicDampingFactor = 0.3;
    controls.addEventListener('change', render);
}


function render() {
    renderer.render(scene, camera);
}

function animate() {
    controls.update();
    requestAnimationFrame(animate);
    renderer.render(scene, camera);
}

function draw() {
    init_scene();
    init_camera();
    init_renderer();
    init_light();
    init_model();
    init_controls();
    animate();
}


function onload() {
    init_chamber_data();
    draw();
}
