import React from 'react';
import Image from 'next/image';
import styles from '@/styles/Home.module.css'
import raspberry_pi from '@/public/RaspberryPi.png';
import Console from '@/components/console'
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome'
import { icon } from '@fortawesome/fontawesome-svg-core/import.macro'
import { faTrashCan } from '@fortawesome/free-regular-svg-icons'

export default function RaspberryPiInterface() {
    const [rpiConsleText, setRpiConsoleText] = React.useState("Raspberry Pi Console");

    function clearRpiConsole() {
        setRpiConsoleText("");
    }


    function handleDeploy(event: React.FormEvent<HTMLFormElement>) {
        event.preventDefault();
        const data = new FormData(event.currentTarget);
        const ip_addr = data.get('ip_addr');
        console.log(ip_addr);
        console.log("Raspberry Pi ...");
        setRpiConsoleText("Running Raspberry Pi deploy command.");
        fetch('/api/test_api', {
            method: 'POST',
            body: JSON.stringify({ cmd: `./scripts/rpi_deploy.sh ${ip_addr}` })
        }).then(response => {
            console.log("Command executed: ");
            setRpiConsoleText("Uploading wasm..")
            console.log(
                response.json().then((res) => {
                    console.log(res);
                    setRpiConsoleText(res.cmd_res);
                }));
        })
    }

    return (
        <>
            <div className={styles.card_heading}>
                <div className={styles.card_heading_text}>
                    Run Wasm on Raspberry Pi
                </div>
                <div className={styles.card_heading_buttondiv}>
                    <button className={styles.app_icon_button} onClick={clearRpiConsole}>
                        <FontAwesomeIcon icon={faTrashCan} className={styles.app_icon} />
                    </button>
                </div>
            </div>

            <div className={styles.raspberrypi_window}>
                <div className={styles.hardware_window}>
                    <div className={styles.hardware_image}>
                        <Image src={raspberry_pi} width={300} alt="Raspberry Pi" />
                        <div>
                            <form onSubmit={handleDeploy} className={styles.form_input}>
                                <input type="text" id="ip_addr" name="ip_addr" placeholder={"IP Address"} className={styles.form_input_element} />
                                <button className={styles.form_button} type="submit"> Deploy and Run </button>
                            </form>
                        </div>
                    </div>
                    <div className={styles.rpi_console}>
                        <Console rows={12} text_value={rpiConsleText} />
                    </div>
                </div>
            </div>
        </>
    );
}

function handleDeploy(event: React.FormEvent<HTMLFormElement>) {
    event.preventDefault();
    const data = new FormData(event.currentTarget);
    const ip_addr = data.get('ip_addr');
    console.log(ip_addr);
}