
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
    'name-CN':'模型粒度',
    'min_value':0.008,
    'max_value':0.02,
    'step':0.001,
    'default_value':0.02
},{
    'name':'Density',
    'name-CN':'空气密度',
    'min_value':2.899,
    'max_value':2.901,
    'step':0.001,
    'default_value':2.9
},{ 
    'name':'Speed',
    'name-CN':'声波速度',
    'min_value':339.9,
    'max_value':340.1,
    'step':0.1,
    'default_value':340   
},{
    'name':'Freq_start',
    'name-CN':'起始频率',
    'min_value':50,
    'max_value':3000,
    'step':50,
    'default_value':500
},{
    'name':'Freq_end',
    'name-CN':'终止频率',
    'min_value':50,
    'max_value':3000,
    'step':50,
    'default_value':2000
},{
    'name':'Freq_step',
    'name-CN':'频率步长',
    'min_value':10,
    'max_value':100,
    'step':1,
    'default_value':50
},{
    'name':'Chamber_radius',
    'name-CN':'腔体半径',
    'min_value':0.03,
    'max_value':0.06,
    'step':0.001,
    'default_value':0.04
},{
    'name':'Chamber_length',
    'name-CN':'腔体长度',
    'min_value':0.075,
    'max_value':0.15,
    'step':0.001,
    'default_value':0.1
},];
var parameters_value = {}

function dict_deep_copy(src) {
    dst = {};
    for (var key in src) {
	var value = src[key];
	dst[key] = value;
    }
    return dst;
}

function get_parameter_html(parameter) {
    var parameter_html = '' +
	'<div class="row">' +
	'  <div class="col-sm-4">' +
	'    <h3>' + parameter['name-CN'] + '</h3>' + 
	'  </div>' + 
	'  <div class="col-sm-8"  style="margin-top:13px">' +
	'    <input id="slider-' + parameter['name'] + '-input"' +
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
	$('#slider-' + parameters_data[i]['name'] + '-input').slider({
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
    data['radius'] = 0.02;
    data['length'] = 0.05;
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
		'    <input id="slider-radius-' + i.toString() + '" data-slider-id="slider-radius-' + i.toString() + '" type="text" data-slider-min="0.01" data-slider-max="0.06" data-slider-step="0.001" data-slider-value="' + chamber_data[i-1]['radius'] + '"/>' +
		'  </div>' +
		'</div>';
	    var chamber_length_html = '' +
		'<div class="row">' +
		'  <div class="col-sm-3 col-sm-offset-1">' +
		'    <h4>Length</h4>' +
		'  </div>' +
		'  <div class="col-sm-8" style="margin-top:13px">' +
		'    <input id="slider-length-' + i.toString() + '" data-slider-id="slider-length-' + i.toString() + '" type="text" data-slider-min="0.01" data-slider-max="0.15" data-slider-step="0.001" data-slider-value="' + chamber_data[i-1]['length'] + '"/>' +
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

const skyblue = '#47DAFF'
const red = '#FF0000'
var freq_tl_data = [];
var freq_tl_data_index;
var freq_tl_target_data = [];
function chart_setdata(freq_tl) {
    freq_tl_data = [];
    for (var freq_str in freq_tl) {
	var tl_str = freq_tl[freq_str];
	var freq = parseInt(freq_str);
	var tl = parseFloat(tl_str);
	freq_tl_data.push({x:freq, y:tl, color:skyblue})
    }
    chart.series[0].setData(freq_tl_data);
}
function chart_setnewdata(freq_tl) {
    console.log(freq_tl);
    freq_tl_target_data = []
    for (var i=0; i<freq_tl.length; i++) {
	var freq_str = freq_tl[i]['freq'];
	var tl_str = freq_tl[i]['tl'];
	var freq = parseInt(freq_str);
	var tl = parseFloat(tl_str);
	freq_tl_target_data.push({x:freq, y:tl, color:red})
    }
    console.log(freq_tl_target_data);
    chart.series[1].setData(freq_tl_target_data);
}

function init_preview_click() {
    $(document).ready(function() {
	$('#preview-btn').click(function() {
	    preview_model(); 
	});
    });
}

function preview_model() {
    var post_data = {};
    // newly added
    chamber_data = [];
    chamber_1 = {};
    chamber_1['radius'] = parameters_value['Chamber_radius'];
    chamber_1['length'] = parameters_value['Chamber_length'];
    chamber_data.push(get_default_copy()); 
    chamber_data.push(chamber_1);
    chamber_data.push(get_default_copy());
    ////////////////////////
    post_data['chambers'] = chamber_data;
    post_data['parameters'] = parameters_value;
    $.post('preview_model/', JSON.stringify(post_data), preview_model_callback);
}

function preview_model_callback(data) {
    if (data['result'] == 'success') {
	surf_file = data['surf-file'];
	update_model(surf_file, 'vtk');
    } else {
	alert('Preview Fail');
    }
}

function init_simulate_click() {
    $('#simulate-btn').click(function() {
	simulate_model();
    });
}

function simulate_model() {
    var post_data = {};
    // newly added
    chamber_data = [];
    chamber_1 = {};
    chamber_1['radius'] = parameters_value['Chamber_radius'];
    chamber_1['length'] = parameters_value['Chamber_length'];
    chamber_data.push(get_default_copy()); 
    chamber_data.push(chamber_1);
    chamber_data.push(get_default_copy());
    ////////////////////////
    post_data['chambers'] = chamber_data;
    post_data['parameters'] = parameters_value;
    $.post('simulate_model/', JSON.stringify(post_data), simulate_model_callback);
}

function simulate_model_callback(data) {
    if (data['result'] == 'success') {
	disable_controls();
	// $('#url-modal-body').html('Please get result from the URL below:<br>' + data['url'])
	$('#url-modal-body').html('请复制下列地址，获取仿真结果：<br>' + data['url'])
	$('#url-modal').modal('show');
    } else {
	alert('Simulate Failed');
    }
}

function init_modal() {
    $('#url-modal').on('hidden.bs.modal', function() {
	enable_controls();
    });
    $('#inverse-modal').on('hidden.bs.modal', function() {
	enable_controls();
    });
    function str2json(freq_tl_str) {
	var freq_tl_json = [];
	var strings = freq_tl_str.split('\n');
	for (var i=0; i<strings.length; i++) {
	    if (strings[i].length > 10) {
		var freq_str = strings[i].split('\t ')[0].split(':')[1];
		var tl_str = strings[i].split('\t ')[1].split(':')[1];
		var freq_tl = {}
		freq_tl['freq'] = freq_str;
		freq_tl['tl'] = tl_str;
		freq_tl_json.push(freq_tl);
	    }
	}
	return freq_tl_json;
    }
    $('#inverse-modal-ok-btn').click(function() {
	var freq_tl = str2json($('#inverse-modal-form').val());
	chart_setnewdata(freq_tl);
	$.post('inverse_sim/', JSON.stringify(freq_tl), inverse_sim_callback);
    });
}

function init_quick_sim_click() {
    $('#quick-sim-btn').click(function() {
	quick_sim_model();
    });
}

function quick_sim_model() {
    var post_data = {};
    // newly added
    chamber_data = [];
    chamber_1 = {};
    chamber_1['radius'] = parameters_value['Chamber_radius'];
    chamber_1['length'] = parameters_value['Chamber_length'];
    chamber_data.push(get_default_copy()); 
    chamber_data.push(chamber_1);
    chamber_data.push(get_default_copy());
    ////////////////////////
    post_data['chambers'] = chamber_data;
    post_data['parameters'] = parameters_value;
    $.post('quick_sim_model/', JSON.stringify(post_data), quick_sim_model_callback);
}

function quick_sim_model_callback(data) {
    if (data['result'] == 'success') {
	chart_setdata(data['freq_tl']);
    } else {
	alert('Quick Simulation Failed!');
    }
}

function single_sim(freq) {
    var post_data = {};
    // newly added
    chamber_data = [];
    chamber_1 = {};
    chamber_1['radius'] = parameters_value['Chamber_radius'];
    chamber_1['length'] = parameters_value['Chamber_length'];
    chamber_data.push(get_default_copy()); 
    chamber_data.push(chamber_1);
    chamber_data.push(get_default_copy());
    ////////////////////////
    post_data['chambers'] = chamber_data;
    post_data['parameters'] = dict_deep_copy(parameters_value);
    post_data['parameters']['Freq_start'] = freq;
    post_data['parameters']['Freq_end'] = freq + 1;
    post_data['parameters']['Freq_step'] = 50;
    $.post('single_sim/', JSON.stringify(post_data), single_sim_callback); 
}

function single_sim_callback(data) {
    if (data['result'] == 'success') {
	// var freq = freq_tl_data[freq_tl_data_index]['x'].toString();
	// var tl = parseFloat(data['freq_tl'][freq]);
	// freq_tl_data[freq_tl_data_index]['color'] = '#FF0000';
	// freq_tl_data[freq_tl_data_index]['y'] = tl;
	// chart.series[0].setData(freq_tl_data);
	
	update_model(data['json-file']['real-surf'], 'json');
    } else {
	alert('Quick Simulation Failed!');
    }    
}

function init_inverse_click() {
    $('#inverse-btn').click(function() {
	disable_controls();
	$('#inverse-modal-form').attr('rows', freq_tl_data.length.toString());
	var html = '';
	for (var i=0; i<freq_tl_data.length; i++) {
	    html += 'freq:' + freq_tl_data[i].x.toString() + '\t tl:' + freq_tl_data[i].y.toFixed(5).toString() + '\n'; 
	}
	$('#inverse-modal-form').val(html);
	$('#inverse-modal').modal('show');
    });
}

function inverse_sim_callback(data) {
    if (data['result'] == 'success') {
	fit_chamber = data['fit-chamber'];
	$('#slider-Chamber_radius-input').slider('setValue', fit_chamber['radius'].toFixed(3));
	$('#slider-Chamber_length-input').slider('setValue', fit_chamber['length'].toFixed(3));
	preview_model();
	quick_sim_model();
	alert('The most suitable chamber is \nradius:' + fit_chamber['radius'].toFixed(3).toString() + ' length:' + fit_chamber['length'].toFixed(3).toString());
    } else {
	alert('Inverse Failed');
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
	    // text: 'Transition Loss of Muffler Under Different Frequency'
	    text: '不同频率下的消声器传输损耗'
	},
	xAxis: {
	    title: {
		// text: 'Frequency (Hz)'
		text: '频率（赫兹）'
	    },
	    type: 'linear'
	},
	yAxis: {
	    title: {
		// text: 'Transition Loss (dB)'
		text: '传输损耗（分贝）'
	    },
	    type: 'linear'
	},
	plotOptions: {
	    series: {
		cursor: 'pointer',
		point: {
		    events: {
			click: function () {
			    chart_click(this);
			}
		    }
		},
		lineWidth: 4
	    }
	},
	series: [{
	    // name: 'muffler',
	    name: '消声器',
	    data: [{x:0, y:0, color:skyblue}, {x:1, y:1, color:skyblue}, {x:2, y:0, color:skyblue}, {x:3, y:1, color:skyblue},  {x:4, y:0, color:skyblue}],
	    color: skyblue,
	    lineWidth: 4
	}, {
	    name: '目标函数', 
	    data: [],
	    color: red
	}]
    });
}

function chart_click(point) {
    var freq = point.x;
    freq_tl_data_index = point.index;
    single_sim(freq);
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

var loader;
var material;
var mesh;
function init_model(input_file, type) {

    if (type == 'json') {
	material = new THREE.MeshLambertMaterial({
	    side: THREE.DoubleSide,
	    color: 0xF5F5F5,
	    vertexColors: THREE.VertexColors
	});

	loader = new THREE.BufferGeometryLoader();
	loader.load(input_file, function(geometry) {
	    geometry.computeVertexNormals();
	    geometry.normalizeNormals();	
	    geometry.computeBoundingBox();
	    var bounding = geometry.boundingBox;
	    
	    var lutColors = []

	    lookup_table = new THREE.Lut('cooltowarm', 512);
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
	    mesh.position.x = -(bounding.min.x + bounding.max.x)/2;
	    mesh.position.y = -(bounding.min.y + bounding.max.y)/2;
	    mesh.position.z = -(bounding.min.z + bounding.max.z)/2;
	    mesh.scale.multiplyScalar(1); 
	    scene.add(mesh); 
	}); 
    }

    if (type == 'vtk') {
	material = new THREE.MeshLambertMaterial({
	    side: THREE.DoubleSide,
	    color: 0xF5F5F5,
	    wireframe: true
	});
	
	loader = new THREE.VTKLoader();
	loader.load(input_file, function(geometry) {
	    geometry.computeVertexNormals();
	    geometry.computeBoundingBox();
	    var bounding = geometry.boundingBox;
	    
	    mesh = new THREE.Mesh( geometry, material );
	    mesh.position.x = -(bounding.min.x + bounding.max.x)/2;
	    mesh.position.y = -(bounding.min.y + bounding.max.y)/2;
	    mesh.position.z = -(bounding.min.z + bounding.max.z)/2;
	    scene.add( mesh );
	}); 
    }
}
function update_model(input_file, type) {
    scene.remove(mesh);
    init_model(input_file, type);
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
function disable_controls() {
    controls.enabled = false; 
}
function enable_controls() {
    controls.enabled = true;
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
    // init_model('static/model/bunny.vtk', 'vtk');
    preview_model();
    quick_sim_model();
    init_controls();
    animate();
}

function onload() {
    init_parameters();
    // init_chamber_data();
    init_preview_click();
    init_quick_sim_click();
    init_simulate_click();
    init_inverse_click();
    init_modal();
    init_chart();
    draw();
}
