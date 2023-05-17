import React from 'react';
import Image from 'next/image';
import styles from '@/styles/Home.module.css'
import esp_image from '@/public/ESP.png';
import Console from '@/components/console'
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome'
import { icon } from '@fortawesome/fontawesome-svg-core/import.macro'
import { faTrashCan } from '@fortawesome/free-regular-svg-icons'

export default function Esp32Interface() {
    const [espConsleText, setEspConsoleText] = React.useState("ESP32 Console");

    function clearEspConsole() {
        setEspConsoleText("");
    }

    function handleDeploy(event: React.FormEvent<HTMLFormElement>) {
        event.preventDefault();
        const data = new FormData(event.currentTarget);
        const ip_addr = data.get('ip_addr');
        console.log(ip_addr);
        console.log("ESP32...");
        setEspConsoleText("Running ESP32 deploy command.");
            fetch('/api/test_api', {
                method: 'POST',
                body: JSON.stringify({ cmd: `./scripts/esp32.sh ${ip_addr}` })
            }).then(response => {
                console.log("Command executed: ");
                setEspConsoleText("Uploading wasm..")
                console.log(
                    response.json().then((res) => {
                        console.log(res);
                        setEspConsoleText(res.cmd_res);
                    }));
            })
    }
    
    return (
        <>
            <div className={styles.card_heading}>
                <div className={styles.card_heading_text}>
                    Run Wasm on ESP32 development board
                </div>
                <div className={styles.card_heading_buttondiv}>
                    <button className={styles.app_icon_button} onClick={clearEspConsole}>
                        <FontAwesomeIcon icon={faTrashCan} className={styles.app_icon} />
                    </button>
                </div>
            </div>

            <div className={styles.raspberrypi_window}>
                <div className={styles.hardware_window}>
                    <div className={styles.hardware_image}>
                        <Image src={esp_image} width={300} alt="ESP32" />
                        <div>
                            <form onSubmit={handleDeploy} className={styles.form_input}>
                                <input type="text" id="ip_addr" name="ip_addr" placeholder={"IP Address"} className={styles.form_input_element} />
                                <button className={styles.form_button} type="submit"> Deploy and Run </button>
                            </form>
                        </div>
                    </div>
                    <div className={styles.rpi_console}>
                        <Console rows={12} text_value={espConsleText} />
                    </div>
                </div>
            </div>
        </>
    );
}