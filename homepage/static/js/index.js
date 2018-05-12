
padding = 10;

window_width = window.innerWidth;
window_height = window.innerHeight;

document.getElementById('whole-container').style.width = window_width.toString() + 'px';
document.getElementById('whole-container').style.height = window_height.toString() + 'px';

width = 3 / 4 * window_width;
height = 5 / 8 * window_height;

document.getElementById('canvas-container').style.width = width.toString() + 'px';
document.getElementById('canvas-container').style.height = height.toString() + 'px';
document.getElementById('control-container').style.paddingRight = padding;
document.getElementById('control-container').style.paddingLeft = padding;
document.getElementById('control-container').style.width = (window_width - width - 2*padding).toString() + 'px';
document.getElementById('control-container').style.height = window_height.toString() + 'px';


// Below are parameter part

var parameters_data = [{
    'name':'Mesh_size',
    'min_value':0.008,
    'max_value':0.02,
    'step':0.001,
    'default_value':0.02
},{
    'name':'Density',
    'min_value':0.5,
    'max_value':4.0,
    'step':0.01,
    'default_value':2.9
},{ 
    'name':'Speed',
    'min_value':200,
    'max_value':1000,
    'step':1,
    'default_value':340   
},{
    'name':'Freq_start',
    'min_value':50,
    'max_value':3000,
    'step':50,
    'default_value':1400
},{
    'name':'Freq_end',
    'min_value':50,
    'max_value':3000,
    'step':50,
    'default_value':1550
},{
    'name':'Freq_step',
    'min_value':10,
    'max_value':100,
    'step':1,
    'default_value':50
},];
var parameters_value = {}

function get_parameter_html(parameter) {
    var parameter_html = '' +
	'<div class="row">' +
	'  <div class="col-sm-4">' +
	'    <h3>' + parameter['name'] + '</h3>' + 
	'  </div>' + 
	'  <div class="col-sm-8"  style="margin-top:13px">' +
	'    <input id="slider-' + parameter['name'] + '"' +
	'      data-slider-id="slider-' + parameter['name'] + '" type="text" ' +
	'      data-slider-min="' + parameter['min_value'].toString() + '"' +
	'      data-slider-max="' + parameter['max_value'].toString() + '"' +
	'      data-slider-step="' + parameter['step'].toString() + '"' +
	'      data-slider-value="' + parameter['default_value'].toString() + '"' +
	'    />' + 
	'  </div>' +
	'</div>';
    return parameter_html;
}

function init_parameters() {
    // init parameters_value
    for (var i=0; i<parameters_data.length; i++) {
	parameters_value[parameters_data[i]['name']] = 0;
    }
    // init '#parameter-ctrl-panel'
    var html = '';
    for (var i=0; i<parameters_data.length; i++) {
	html += get_parameter_html(parameters_data[i]);
    }
    $('#parameter-ctrl-panel').html(html);
    // init parameter sliders
    for (var i=0; i<parameters_data.length; i++) {
	$('#slider-' + parameters_data[i]['name']).slider({
	    formatter: function(value) {
		var id = $(this).attr('id');
		var strs = id.split('-');
		var name = strs[strs.length-1];
		parameters_value[name] = value;
		return value;
	    }
	});
    }
}


// Below are chamber part

var chamber_data = [];
var max_mesh_size = 0.01;
function get_default_copy() {
    var data = {};
    data['radius'] = 0.05;
    data['length'] = 0.1;
    return data;
}

function init_chamber_data() {
    chamber_data.push(get_default_copy());
    update_chamber_data();
}

function init_chamber_sliders() {
    // chamber sliders
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
}

function init_other_sliders() {
    // max mesh size slider
    $('#slider-max-mesh-size').slider({
	formatter: function(value) {
	    max_mesh_size = value;
	    return value;
	}
    });
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

	init_chamber_sliders();
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


// Below are click part

function init_preview_click() {
    $(document).ready(function() {
	$('#preview-btn').click(function() {
	    preview_model(); 
	});
    });
}

function preview_model() {
    var post_data = {};
    post_data['chambers'] = chamber_data;
    post_data['parameters'] = parameters_value;
    $.post('preview_model/', JSON.stringify(post_data), preview_model_callback);
}

function preview_model_callback(data) {
    if (data['result'] == 'success') {
	surf_file = data['surf-file'];
	update_model(surf_file);
    } else {
	alert('Preview Fail');
    }
}

function init_apply_click() {
    $('#simulate-btn').click(function() {
	simulate_model();
    });
}

function simulate_model() {
    var post_data = {};
    post_data['chambers'] = chamber_data;
    post_data['parameters'] = parameters_value;
    $.post('simulate_model/', JSON.stringify(post_data), simulate_model_callback);
}

function simulate_model_callback(data) {
    if (data['result'] == 'success') {
	var freq_tl = data['freq_tl'];
	freq_tl_data = []
	for (var freq_str in freq_tl) {
	    tl_str = freq_tl[freq_str];
	    freq = parseInt(freq_str);
	    tl = parseFloat(tl_str);
	    freq_tl_data.push([freq, tl])
	}
	chart.series[0].setData(freq_tl_data);
    } else {
	alert('Simulate Failed');
    }
}


// Below are chart part

var chart;
function init_chart() {
    chart = Highcharts.chart('function', {
        chart: {
            type: 'spline'
        },
        title: {
            text: 'Transition Loss of Muffler Under Different Frequency'
        },
        xAxis: {
	    title: {
		text: 'Frequency (Hz)'
            },
	    type: 'linear'
        },
        yAxis: {
	    title: {
		text: 'Transition Loss (dB)'
	    },
	    type: 'linear'
	},
	plotOptions: {
            series: {
		cursor: 'pointer',
		point: {
                    events: {
			click: function () {
                            alert('Frequency: ' + this.x + ', TL: ' + this.y);
			}
                    }
		}
            }
	},
        series: [{
            name: 'muffler',
            data: [[0, 0], [1, 1], [2, 0], [3, 1], [4, 0]]
	}]
    });
}


// Below are Three.js part

var scene;
function init_scene() {
    scene = new THREE.Scene();
    scene.background = new THREE.Color( 0xffffff );
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
    var ambient_light = new THREE.AmbientLight("#8c8c8c");
    scene.add(ambient_light);
    light = new THREE.DirectionalLight(0xffffff);
    light.position.set(200, 200, 1000).normalize();
    //camera.add(light);
    //camera.add(light.target);
}

var material;
var loader = new THREE.BufferGeometryLoader();
var mesh;
function init_model(input_file) {
    material = new THREE.MeshLambertMaterial({
	side: THREE.DoubleSide,
	color: 0xF5F5F5,
	vertexColors: THREE.VertexColors
    });

    loader.load(input_file, function(geometry) {
	geometry.computeVertexNormals();
	geometry.normalizeNormals();
	
	var lutColors = []

	lookup_table = new THREE.Lut('rainbow', 512);
	lookup_table.setMax(1);
	lookup_table.setMin(0);
	for (var i=0; i<geometry.attributes.value.array.length; i++) {
	    var colorValue = geometry.attributes.value.array[i];
	    var color = lookup_table.getColor(colorValue);
	    lutColors[3*i  ] = color.r;
	    lutColors[3*i+1] = color.g;
	    lutColors[3*i+2] = color.b;
	}
	geometry.addAttribute('color', new THREE.BufferAttribute(new Float32Array(lutColors), 3));
	
	mesh = new THREE.Mesh(geometry, material);
	mesh.position.set(0, 0, 0);
	mesh.scale.multiplyScalar(1);
	scene.add(mesh);
    });
}
function update_model(input_file) {
    scene.remove(mesh);
    init_model(input_file);
}

var controls;
function init_controls() {
    controls = new THREE.TrackballControls(camera);
    controls.rotateSpeed = 5;
    controls.zoomSpeed = 3;
    controls.panSpeed = 0.8;
    controls.staticMoving = true;
    controls.dynamicDampingFactor = 0.3;
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
    init_model('static/model/muffler_imag_1400_surf.json');
    //init_model('static/model/surface.vtk');
    init_controls();
    animate();
}

function onload() {
    init_parameters();
    init_chamber_data();
    init_preview_click();
    init_apply_click();
    init_chart();
    draw();
}
