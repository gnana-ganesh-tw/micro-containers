import * as React from 'react';
import { Component, FunctionComponent } from 'react';
import Console from '@/components/console'
import styles from '@/styles/Home.module.css'
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome'
import { icon } from '@fortawesome/fontawesome-svg-core/import.macro'
import { faTrashCan } from '@fortawesome/free-regular-svg-icons'
import { faTerminal } from '@fortawesome/free-solid-svg-icons'
import { faArrowsTurnToDots } from '@fortawesome/free-solid-svg-icons'
import type { NextApiRequest, NextApiResponse } from 'next'


export default function Pipeline() {
    let consoleText = "Build Console";

    // 0: Console, 1: pipeline
    const [buildConsoleText, setBuildConsoleText] = React.useState(consoleText);

    function clearBuildConsole() {
        setBuildConsoleText("");
    }

    function execLocalBuild() {
        console.log("Local build ...");
        setBuildConsoleText("Running build local command.");
        fetch('/api/test_api', {
            method: 'POST',
            body: JSON.stringify({ cmd: "./scripts/build_local.sh" })
        }).then(response => {
            console.log("Command executed: ");
            console.log(
                response.json().then((res) => {
                    console.log(res);
                    setBuildConsoleText(res.cmd_res);
                }));
        })
    }

    function ActiveView({ consoleText }: { consoleText: string }) {

        return (
            <>
                <div className={styles.build_console}>
                    <Console rows={16} text_value={consoleText} />
                </div>
                <div className={`w3-row-padding`}>
                    <button className={styles.global_button} onClick={execLocalBuild}> Build locally </button>
                </div>
            </>
        );
    }

    return (
        <>
            <div className={styles.card_heading}>
                <div className={styles.card_heading_text}>
                    Build the Traffic Light Controller Wasm app
                </div>
                <div className={styles.card_heading_buttondiv}>
                    <button className={styles.app_icon_button} onClick={clearBuildConsole}>
                        <FontAwesomeIcon icon={faTrashCan} className={styles.app_icon} />
                    </button>
                </div>
            </div>
            <div className={styles.pipeline_window}>
                <ActiveView consoleText={buildConsoleText} />
            </div>
        </>

    );
}