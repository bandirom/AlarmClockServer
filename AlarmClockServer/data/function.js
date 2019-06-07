var xmlHttp = createXmlHttpObject();
function createXmlHttpObject() {
    if (window.XMLHttpRequest) {
        xmlHttp = new XMLHttpRequest();
    } else {
        xmlHttp = new ActiveXObject('Microsoft.XMLHTTP');
    }
    return xmlHttp;
}
function loadConfig() {
    if (xmlHttp.readyState == 0 || xmlHttp.readyState == 4) {
        xmlHttp.open('PUT', '/configs.json', true);
        xmlHttp.send(null);
        xmlHttp.onload = function (e) {
            jsonResponse = JSON.parse(xmlHttp.responseText);
            loadBlock();
        }
    }
}

function loadInteractive() {
    if (xmlHttp.readyState == 0 || xmlHttp.readyState == 4) {
        xmlHttp.open('PUT', '/InteractiveSetting.json', true);
        xmlHttp.send(null);
        xmlHttp.onload = function (e) {
            jsonResponse = JSON.parse(xmlHttp.responseText);
            loadBlock();
        }
    }
}

function loadBlock(data2) {
    data2 = JSON.parse(xmlHttp.responseText);
    data = document.getElementsByTagName('body')[0].innerHTML;
    var new_string;
    for (var key in data2) {
        new_string = data.replace(new RegExp('{{' + key + '}}', 'g'), data2[key]);
        data = new_string;
    }
    document.getElementsByTagName('body')[0].innerHTML = new_string;
    handleServerResponse();
}

function val(id) {
    var v = document.getElementById(id).value;
    return v;
}
function send_request(submit, server) {
    request = new XMLHttpRequest();
    request.open("GET", server, true);
    request.send();
    save_status(submit, request);
}
function save_status(submit, request) {
    old_submit = submit.value;
    request.onreadystatechange = function () {
        if (request.readyState != 4) return;
        submit.value = request.responseText;
        setTimeout(function () {
            submit.value = old_submit;
            submit_disabled(false);
        }, 1000);
    }
    submit.value = 'Wait please...';
    submit_disabled(true);
}
function submit_disabled(request) {
    var inputs = document.getElementsByTagName("input");
    for (var i = 0; i < inputs.length; i++) {
        if (inputs[i].type === 'submit') { inputs[i].disabled = request; }
    }
}
function toggle(target) {
    var curVal = document.getElementById(target).className;
    document.getElementById(target).className = (curVal === 'hidden') ? 'show' : 'hidden';
}

var set_real_time;
function handleServerResponse() {
    clearTimeout(set_real_time);
    var res = jsonResponse.time.split(":");
    real_time(hours = res[0], min = res[1], sec = res[2]);
    document.body.style.backgroundColor = "rgb(" + jsonResponse.rgb + ")";
}
function real_time(hours, min, sec) {
    sec = Number(sec) + 1;
    if (sec >= 60) { min = Number(min) + 1; sec = 0; }
    if (min >= 60) { hours = Number(hours) + 1; min = 0; }
    if (hours >= 24) { hours = 0 };
    document.getElementById("time").innerHTML = hours + ":" + min + ":" + sec;
    set_real_time = setTimeout("real_time(" + hours + "," + min + "," + sec + ");", 1000);
}
function load_time(submit) {
    server = "/Time";
    send_request(submit, server);
    load();
}
function time_zone(submit) {
    server = "/TimeZone?timezone=" + val('timezone');
    send_request(submit, server);
}
function set_time_zone(submit) {
    var set_date = new Date();
    var gmtHours = -set_date.getTimezoneOffset() / 60;
    document.getElementById('timezone').value = gmtHours;
    server = "/TimeZone?timezone=" + gmtHours;
    send_request(submit, server);
}
function set_ssid(submit) {
    server = "/ssid?ssid=" + val('ssid') + "&password=" + encodeURIComponent(val('password'));
    send_request(submit, server);
    alert("Измененя вступят в силу после перезагрузки. Пожалуйта перезагрузите устройство.");
}
function set_ssid_ap(submit) {
    server = "/ssidap?ssidAP=" + val('ssidap') + "&passwordAP=" + encodeURIComponent(val('passwordAp'));
    send_request(submit, server);
    alert("Измененя вступят в силу после перезагрузки. Пожалуйта перезагрузите устройство.");
}
function set_ssdp(submit) {
    server = "/ssdp?ssdp=" + val('ssdp');
    send_request(submit, server);
    document.getElementById('ssdp_t').innerHTML = val('ssdp');
}
function restart(submit, texts) {
    if (confirm(texts)) {
        server = "/restart?device=ok";
        send_request(submit, server);
        return true;
    } else {
        return false;
    }
}

function sendbutton(state) {
    server = "/device?rele=" + state;
    request = new XMLHttpRequest();
    request.open("GET", server, true);
    request.send();
}

function get_data() {
    var requestURL = "/data.json"
    var request = new XMLHttpRequest();
    request.open('GET', requestURL);
    request.responseType = 'json';
    request.send();
    request.onload = function () {
        var data = request.response;
        populateHeader(superHeroes);
        showHeroes(superHeroes);
    }
}

function SetAlarmTime(submit) {
    server = "/AlarmTime?AlarmTime=" + val('AlarmTime') + "&AlarmDate=" +val('AlarmDate');
    send_request(submit, server);
    document.getElementById('AlarmTime').innerHTML = val('AlarmTime');
}

function ShiftChoice(submit) {
    var shift = document.querySelector('input[name=shift]:checked').value;
    server = "/ShiftSet?shift=" + shift;
    send_request(submit, server);
}

