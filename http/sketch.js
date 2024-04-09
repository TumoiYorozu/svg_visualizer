let maxTime = 0; // timeの最大値を格納する変数
let commands = []; // コマンドを格納する配列
let numDrawCommands = 0; // 描画したコマンド数
let lastFile; // 最後に選択されたファイルの内容を保存する変数
let playButton, playSpeedInput;
let playing = false; // 再生中かどうかのフラグ
let playSpeed = 5; // 再生速度（スライダーのvalueを進める速度）

let final_score = null;
let problem_param = null;

// スライダーの最大値を更新するグローバル関数
window.updateMaxTime = function(maxTime) {
    document.getElementById('timeSlider').max = maxTime;
};

// 現在のスライダーの値を更新するグローバル関数
window.updateSliderValue = function(value) {
    document.getElementById('timeSlider').value = value;
};

function setup() {
    createCanvas(800, 800).parent('canvasContainer'); // キャンバスの親要素を設定
    
    noLoop();

    // // ファイル入力処理
    // document.getElementById('fileInput').addEventListener('change', function(e) {
    //     if (e.target.files.length > 0) {
    //         // ファイルが選択されたとき、そのファイルを保存
    //         lastFile = e.target.files[0];
    //         readFile(lastFile);
    //     }
    // });

    // document.getElementById('fileInput').addEventListener('click', function(e) {
    //     this.value = null;
    // });

    // スライダーの設定部分
    // スライダーの値が変更されたときにテキストボックスを更新
    let timeSlider = document.getElementById('timeSlider');
    timeSlider.addEventListener('input', function() {
        document.getElementById('currentTimeInput').value = this.value;
        if (!playing) {
            redraw(); // スライダーが手動で操作されたときに描画を更新
        }
    });

    // テキストボックスの値が変更されたときにスライダーの値を更新
    let currentTimeInput = document.getElementById('currentTimeInput');
    currentTimeInput.addEventListener('input', function() {
        let value = parseFloat(this.value);
        if (!isNaN(value)) {
            value = Math.min(Math.max(value, 0), maxTime); // 範囲内に収める
            timeSlider.value = value;
            if (!playing) {
                redraw(); // テキストボックスが手動で操作されたときに描画を更新
            }
        }
    });

    // 再生速度の設定（テキストボックスから値を取得）
    document.getElementById('playSpeed').addEventListener('input', function() {
        playSpeed = parseFloat(this.value); // 再生速度を更新
        if (!isNaN(playSpeed) && playSpeed > 0) {
            frameRate(playSpeed);
        }
    });

    // 再生ボタンのクリックイベントをHTML側で扱う
    document.getElementById('playButton').addEventListener('click', togglePlay);
    document.getElementById('prev_flame').addEventListener('click', prev_flame);
    document.getElementById('next_flame').addEventListener('click', next_flame);

    // GIF録画ボタンのイベントリスナーを追加
    // document.getElementById('recordGifButton').addEventListener('click', recordGif);

    // 再生速度設定のセットアップ
    playSpeedInput = select('#playSpeed');
    playSpeedInput.input(() => {
        playSpeed = float(playSpeedInput.value());
        if (!isNaN(playSpeed) && playSpeed > 0) {
            frameRate(playSpeed);
        }
    });
    console.log("playSpeed", playSpeed)
    frameRate(playSpeed);
}

function prev_flame() {
    let timeSlider = document.getElementById('timeSlider');
    const nextTime = parseFloat(timeSlider.value)-1;
    if (nextTime < 0) nextTime = 0;
    timeSlider.value = nextTime
    document.getElementById('currentTimeInput').value = nextTime; //
    draw();
    
}
function next_flame() {
    let timeSlider = document.getElementById('timeSlider');
    const nextTime = parseFloat(timeSlider.value)+1;
    if (nextTime > maxTime) nextTime = maxTime;
    timeSlider.value = nextTime
    document.getElementById('currentTimeInput').value = nextTime; //
    draw();
}

function draw() {
    // clear();
    // background('white');

    if (playing) {
        loop();
        // 描画コマンドの実行とその他の描画処理...
        let timeSlider = document.getElementById('timeSlider');
        // let timeSlider = document.getElementById('currentTimeInput');
        const currentTime = parseFloat(timeSlider.value);
        // let nextTime = currentTime + playSpeed; // 次の時間を計算
        let nextTime = currentTime + 1; // 次の時間を計算

        timeSlider.value = nextTime; // スライダーの値を更新
        if (nextTime >= maxTime) {
            nextTime = maxTime;
            playing = false; // 最大値に達したら再生を停止
            noLoop(); // drawループを停止
        }
        timeSlider.value = (nextTime); // スライダーの値を更新
        document.getElementById('currentTimeInput').value = nextTime; // テキストボックスも更新
   }

    // 描画コマンドの実行
    executeCommands();

    // 全描画コマンド数と表示描画コマンド数の表示
    /*
    fill(0); // テキストの色を黒に設定
    textSize(12); // テキストサイズの設定
    text(`描画コマンド数: ${numDrawCommands}/${commands.length}`, 10, height - 20);
    */
}   
function loadSvgContent(svgContent) {
    const canvasContainer = document.getElementById('canvasContainer');
    canvasContainer.innerHTML = svgContent;
    parseCommands(svgContent);
    draw();
}

function readFile(file) {
    const reader = new FileReader();
    reader.onload = function(event) {
        const svgContent = event.target.result;
        const canvasContainer = document.getElementById('canvasContainer');
        canvasContainer.innerHTML = svgContent;

        // スライダーの最大値と現在値を更新
        let timeSlider = document.getElementById('timeSlider');
        timeSlider.max = maxTime;
        timeSlider.value = 0; // スライダーの値をリセット
        document.getElementById('maxTimeDisplay').innerText = `${maxTime}`; // 最大時間の表示を更新
        // redraw(); // キャンバスを更新
    };

    reader.readAsText(file);
}

function parseCommands(fileContent) {
    commands = []; // コマンドリストを初期化
    maxTime = 0; // 最大time値をリセット

    let beginTime = -1;
    let endTime = -1;
    const commandsArray = fileContent.replace(/\r\n?/g, '\n').split('\n');
    commandsArray.forEach(commandString => {
        const trimmedCommand = commandString.trim();
        if (trimmedCommand === '') return;
        if (trimmedCommand === '</svg>') return;
        const timeMatch = trimmedCommand.match(/<!--time=(-?\d+(?::\d+)?)-->/);
        if (timeMatch) {
            if (timeMatch[1] == "-1") {
                beginTime = -1;
                endTime   = -1;
            } else {
                const time = timeMatch[1].split(':');
                if (time.length == 1) {
                    beginTime = parseInt(time[0]);
                    endTime   = beginTime+1;
                } else {
                    beginTime = parseInt(time[0]);
                    endTime   = parseInt(time[1]);
                }
                maxTime = Math.max(maxTime, endTime-1);
            }
        } else {
            commands.push({ beginTime: beginTime, endTime: endTime, cmd: trimmedCommand });
        }
    });
    // console.log(commands);
    timeSlider.max = maxTime;
    // 最大値を表示
    document.getElementById('maxTimeDisplay').innerText = `${maxTime}`;
}

function executeCommands() {
    let timeSlider = document.getElementById('timeSlider');
    const time = parseInt(timeSlider.value);
    // const startTime = currentTime;
    // const endTime = currentTime;

    // 時間の条件を満たすコマンド文字列を連結
    let commandsToExecute = '';
    numDrawCommands = 0;
    // commands.forEach(({time, cmd}) => {
    //     if ( (startTime <= time && time <= endTime) || time < 0) { // time = -1は常に描画
    //         commandsToExecute += cmd + ';';
    //         numDrawCommands += 1;
    //     }
    // });
    commands.forEach(({beginTime, endTime, cmd}) => {
        if (beginTime < 0 || (beginTime <= time && time < endTime)) { // time = -1は常に描画
            commandsToExecute += cmd + '\n';
            numDrawCommands += 1;
        }
    });
    commandsToExecute += "\n</svg>"
    // console.log(commandsToExecute);
    const canvasContainer = document.getElementById('canvasContainer');
    canvasContainer.innerHTML = commandsToExecute;
    
    if (commandsToExecute) {
        // eval(commandsToExecute); // 連結されたコマンドをevalで1回だけ評価
    }

    // let selectableElements = document.querySelectorAll('[data-txt]');
    // Array.from( selectableElements ).forEach(elem => {
    // elem.addEventListener("mouseover", eve => {
    //     // console.log("txt:", elem.dataset.txt);
    //     hideTooltip();
    //     showTooltip(elem.dataset.txt, eve);
    // });
    
    // // elem.addEventListener("mouseover", eve => {
    //     // hideTooltip();
    // // });
    // // elem.addEventListener("mouseleave", eve => {
    // //   let textId = elem.id + "-text";
    // //   let textElem = document.getElementById(textId);
    // //   if( textElem != null ) {
    // //     textElem.classList.remove("shown");
    // //   }
    // // });
    // });
}
///////
document.addEventListener('DOMContentLoaded', function() {
    document.addEventListener('mouseover', function(event) {
      const target = event.target.closest('[data-txt]');
      if (target) {
        const tooltipText = target.getAttribute('data-txt');
        showTooltip(tooltipText, event);
      }
    });
    document.addEventListener('mouseout', function(event) {
      const target = event.target.closest('[data-txt]');
      if (target) {
        hideTooltip();
      }
    });
});


let tooltipElement = null;

function showTooltip(text, event) {
    // ツールチップ要素を作成
    tooltipElement = document.createElement('div');
    tooltipElement.className = 'tooltip';
    tooltipElement.innerHTML = text;
    document.body.appendChild(tooltipElement);
  
    // ツールチップの位置を設定
    const tooltipHeight = tooltipElement.offsetHeight;
  
    let left = event.pageX;
    let top = event.pageY - tooltipHeight;
  
    tooltipElement.style.left = left + 'px';
    tooltipElement.style.top = top + 'px';
  }
function hideTooltip() {
  if (tooltipElement) {
    // ツールチップ要素を削除
    document.body.removeChild(tooltipElement);
    tooltipElement = null;
  }
}
///////

// 再生/停止の切り替え関数
function togglePlay() {
    playing = !playing;
    if (playing) {
        loop(); // drawループを有効にして描画を続ける
    } else {
        noLoop(); // drawループを停止
    }
}


function getSvg2Canvas (bairitsu = 1) {
    return new Promise((resolve, reject) => {
        const canvasContainer = document.getElementById('canvasContainer');
        const svgElement = canvasContainer.querySelector('svg');
        const svgString = new XMLSerializer().serializeToString(svgElement);
        const canvas = document.createElement('canvas');
        const ctx = canvas.getContext('2d');
        const img = new Image();
        img.onload = function() {
            canvas.width = img.width * bairitsu;
            canvas.height = img.height * bairitsu;
            ctx.drawImage(img, 0, 0, canvas.width, canvas.height);
            resolve(canvas);
        }
        img.onerror = function() {
            reject(new Error('画像の読み込みに失敗しました'));
        }
        img.src = 'data:image/svg+xml;base64,' + btoa(unescape(encodeURIComponent(svgString)));
    });
}

// フレームを保存
function saveFlame(bairitsu = 1) {
    getSvg2Canvas(bairitsu).then(canvas => canvas.toBlob(blob => {
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        const filename = prompt('保存するファイル名を入力してください:', 'flame.png');
        if (filename == null) return;
        a.download = filename;
        a.click();
        URL.revokeObjectURL(url);
    }, 'image/png'));
}

// クリップボードにコピー
function copyToClipboard(bairitsu = 1) {
    getSvg2Canvas(bairitsu).then(canvas => canvas.toBlob(blob => {
        const item = new ClipboardItem({ 'image/png': blob });
        navigator.clipboard.write([item]);
    }, 'image/png'));
    popup_message('クリップボードにコピーしました');
}



function recordGif(bairitsu = 1) {
    // saveGif('mySketch', 5);
    // alert("recordGif 未実装！");
    
    const save_gif = document.getElementById("save_gif");
    save_gif.disabled = true;
    // save_gif.value = "Generating GIF...";
    const max_turn = maxTime;
    // const step = 20;
    // const delay = step * 500 / speed.value;
    const delay = 1000 / playSpeed;

    const gif = new GIF({
        workers: 2,
        quality: 10,
    });
    gif.on("progress", function (p) {
        // save_gif.value = String(Math.round(50 + 50 * p)).padStart(3, " ") + "% finished";
    });

    let timeSlider = document.getElementById('timeSlider');
    function add_frame(t) {
        // save_gif.value = String(Math.round(50.0 * t / max_turn)).padStart(3, " ") + "% finished";
        timeSlider.value = t;
        draw();
        getSvg2Canvas(bairitsu).then(canvas => {
            if (t < max_turn) {
                gif.addFrame(canvas, { delay: delay });
                add_frame(t+1);
            } else {
                gif.addFrame(canvas, { delay: 3000 });
                gif.on('finished', function (blob) {
                    const a = document.createElement("a");
                    a.href = URL.createObjectURL(blob);
                    a.download = "vis.gif";
                    a.click();
                    window.URL.revokeObjectURL(a.href);
                    // save_gif.value = "Save as Animation GIF";
                    save_gif.disabled = false;
                });
                gif.render();
            }
        });
    }
    add_frame(0);
}



const repeat_cooldown = 500;
let now_repeat_id = 0;
let RIGHT_ARROW_PRESSED = 0;
let LEFT_ARROW_PRESSED = 0;
function repeat_key(repeat_id, next) {
    if (repeat_id != now_repeat_id) return;
    if (next) {
        next_flame();
        if (RIGHT_ARROW_PRESSED) setTimeout(repeat_key, 70, repeat_id, 1);
    } else {
        prev_flame();
        if (LEFT_ARROW_PRESSED) setTimeout(repeat_key, 70, repeat_id, 0);
    }

}

function keyPressed() {
    if (keyCode === RIGHT_ARROW) {
        RIGHT_ARROW_PRESSED=1;
        next_flame();
        setTimeout(repeat_key, 600, now_repeat_id, 1);
    }
    if (keyCode === LEFT_ARROW) {
        LEFT_ARROW_PRESSED=1;
        prev_flame();
        setTimeout(repeat_key, 600, now_repeat_id, 0);
    }
    if (key == 'a') {
        const nextTime = 0;
        document.getElementById('timeSlider').value = nextTime
        document.getElementById('currentTimeInput').value = nextTime;
        draw();
    }
    if (key == 'R') {
        // load_default_svg();
    }
    
}


function keyReleased() {
    if (keyCode === RIGHT_ARROW) {
        ++now_repeat_id;
        RIGHT_ARROW_PRESSED=0;
    }
    if (keyCode === LEFT_ARROW) {
        ++now_repeat_id;
        LEFT_ARROW_PRESSED=0;
    }
}

const socket = new WebSocket(`ws://${window.location.host}/ws`);

socket.addEventListener('open', () => {
    console.log('WebSocket connection established');
});

socket.addEventListener('message', (event) => {
    const message = event.data;
    console.log('Received message:', message);
    if (message.startsWith('close')) {
        const autoReloadCheckbox = document.getElementById('autoReloadCheckbox');
        if (autoReloadCheckbox.checked) {
            const fileSelect = document.getElementById('fileSelect');
            const selectedFile = fileSelect.value;
            const closeFile = message.split(':')[1].trim();
            if (selectedFile === closeFile) {
                fetch_svg(selectedFile, true);
                popup_message('自動更新されました');
            }
        }
    }
});

function popup_message(message) {
    const autoReloadMessage = document.getElementById('message-container');
    autoReloadMessage.innerText = message;
    // document.body.appendChild(autoReloadMessage);
    setTimeout(() => {
        autoReloadMessage.classList.add('slide-in');
        setTimeout(() => {
            autoReloadMessage.classList.remove('slide-in');
        }, 2000);
    }, 1);
}

function fetch_svgs() {
    fetch('/svg/')
        .then(response => response.text())
        .then(fileList => {
            const fileSelect = document.getElementById('fileSelect');
            fileSelect.innerHTML = ''; // Clear the existing options
            const files = fileList.split('\n');
            files.forEach(file => {
                const option = document.createElement('option');
                option.value = file;
                option.text = file;
                fileSelect.appendChild(option);
            });
            // Trigger the "change" event
            fileSelect.dispatchEvent(new Event('change'));
        })
        .catch(error => {
            console.error('Failed to fetch SVG files:', error);
        });
}

fetch_svgs();

document.getElementById('fileSelect').addEventListener('change', function() {
    const selectedFile = this.value;
    console.log('Selected file:', selectedFile);
    fetch_svg(selectedFile);
});

function fetch_svg(filename, auto_reloaded=false) {
    fetch('/svg/' + filename)
        .then(response => response.text())
        .then(svgContent => {
            loadSvgContent(svgContent);
            const loadTimeElement = document.getElementById('load_time');
            const currentTime = new Date();
            const hours = currentTime.getHours().toString().padStart(2, '0');
            const minutes = currentTime.getMinutes().toString().padStart(2, '0');
            const seconds = currentTime.getSeconds().toString().padStart(2, '0');
            const message = `更新: ${hours}:${minutes}:${seconds}`
            // if (auto_reloaded) {
            //     message += ' 自動更新';
            // } else {
            //     message += ' 更新';
            // }
            loadTimeElement.textContent = message;
        })
        .catch(error => {
            console.error('Failed to fetch SVG file:', error);
        });
}