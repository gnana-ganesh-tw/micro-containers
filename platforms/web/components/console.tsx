import styles from '@/styles/Home.module.css'
import React from 'react';

const cmd_script_test = 'sh scripts/test.sh';

const cmd_script_reset = 'echo "Hello world"';
const cmd_script_rpi = 'echo Start uploading..\n' +
  'scp -r public/turn_indicator.wasm  pi@192.168.29.175:/home/pi/wasm/\n' +
  'echo Upload done!\n';

const cmd_script_esp32 = "echo Start uploading..\n" +
  'curl -d "filename=turn_indicator.wasm" -X POST http://10.137.124.121/task/stop\n' +
  'curl -F "filename=@public/turn_indicator.wasm" -X POST http://10.137.124.121/task/upload\n' +
  'curl -d "filename=turn_indicator.wasm" -X POST http://10.137.124.121/task/start\n' +
  'echo Upload done!\n';

export default function Console({ rows, text_value }) {

  const [console_txt, setConsoleText] = React.useState('Console text');

  async function execCommand(cmd: number) {
    let cmd_script = "echo 'No command selected'\n"
    if (cmd == 1) {
      cmd_script = cmd_script_test;
    } else if (cmd == 2) {
      cmd_script = cmd_script_rpi;
    } else if (cmd == 3) {
      cmd_script = cmd_script_esp32;
    } else if (cmd == 4) {
      cmd_script = cmd_script_reset;
    }

    fetch('/api/test_api', {
      method: 'POST',
      body: JSON.stringify({
        cmd: cmd_script
      }),
    }).then((res) => {
      console.log(res);
      res.json().then((res) => {
        console.log(res);
        setConsoleText(res.cmd_res);
      });
    });

  }

  return (
    <div className={styles.console_window}>
      <div className={styles.console_content}>
        <textarea className={styles.hardware_console} readOnly={true}
          rows={rows} value={text_value} />
      </div>
    </div>
  )
}
