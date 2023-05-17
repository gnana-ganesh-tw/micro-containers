import Image from "next/image"
import right_empty from "@/public/RightEmptyArrow.svg"
import right_filled from "@/public/RightSolidArrow.svg"
import left_empty from "@/public/LeftEmptyArrow.svg"
import left_filled from "@/public/LeftSolidArrow.svg"
import parking_button from "@/public/ParkingBlinker.svg"
import { useState } from "react"
import { TurnIndicatorControllerInterface } from "@/app_lib/ts_wasm_glue"
import styles from "@/styles/Home.module.css"

/*
    indicatorStatus : 
        0 - Left and Right indicators OFF
        1 - Left indicator ON, Right indicator is OFF
        2 - Right indicator ON, Left indicator is OFF
        3 - Left and Right indicators ON (Parking mode is active)
*/

let leftBlinker: {
    active: boolean;
    intervalHandle: NodeJS.Timeout | undefined;
} = { active: false, intervalHandle: undefined };

let rightBlinker: {
    active: boolean;
    intervalHandle: NodeJS.Timeout | undefined;
} = { active: false, intervalHandle: undefined };

export default function Simulator({ right = false, left = false }: { right: boolean, left: boolean }) {
    const [rightState, setRightState] = useState(right);
    const [leftState, setLeftState] = useState(left);
    const [indicatorStatus, setIndicatorStatus] = useState(0);
    const turnIndicatorController = new TurnIndicatorControllerInterface(updateIndicatorStatus);

    function leftClicked() {
        turnIndicatorController.leftButtonPressed(indicatorStatus);
    }

    function rightClicked() {
        turnIndicatorController.rightButtonPressed(indicatorStatus);
    }

    function parkingClicked() {
        turnIndicatorController.parkingButtonPressed(indicatorStatus);
    }

    function updateLeftBlinker(newState: boolean, interval: number) {
        if (newState == false || interval == 0) {
            setLeftState(newState);
            clearInterval(leftBlinker.intervalHandle);
            leftBlinker.active = false;
            return;
        }
        if (leftBlinker.active) {
            return;
        }
        let state = true;
        setLeftState(state);
        leftBlinker.active = true;
        leftBlinker.intervalHandle = setInterval(() => {
            state = !state;
            setLeftState(state);
        }, interval);
    }

    function updateRightBlinker(newState: boolean, interval: number) {
        if (newState == false || interval == 0) {
            setRightState(newState);
            clearInterval(rightBlinker.intervalHandle);
            rightBlinker.active = false;
            return;
        }
        if (rightBlinker.active) {
            return;
        }
        let state = true;
        setRightState(state);
        rightBlinker.active = true;
        rightBlinker.intervalHandle = setInterval(() => {
            state = !state;
            setRightState(state);
        }, interval);
    }

    function updateIndicatorStatus(newStatus: number, mode: number) {
        console.log("Indicator status:" + newStatus + " Mode: " + mode);
        setIndicatorStatus(newStatus);
        switch (newStatus) {
            case 0:
                updateLeftBlinker(false, 0);
                updateRightBlinker(false, 0);
                break;
            case 1:
                updateLeftBlinker(true, mode);
                updateRightBlinker(false, 0);
                break;
            case 2:
                updateLeftBlinker(false, 0);
                updateRightBlinker(true, mode);
                break;
            case 3:
                updateLeftBlinker(true, mode);
                updateRightBlinker(true, mode);
                break;
        }
    }

    return (
        <div className={`w3-cell-row`}>
            <div className={styles.indicator_image}>
                <LeftIndicator on={leftState} />
                <button onClick={leftClicked} className={styles.indicator_button}>
                    Left
                </button>
            </div>

            <div className={styles.parking_button_cell}>
                <button className={styles.parking_button}> <Image src={parking_button} onClick={parkingClicked}
                    width={128} height={128} alt="Parking button" /> </button>
            </div>

            <div className={styles.indicator_image}>
                <RightIndicator on={rightState} />
                <button onClick={rightClicked} className={styles.indicator_button}>
                    Right
                </button>
            </div>

        </div>
    );
}

export function RightIndicator({ on }: { on: boolean }) {
    return (
        <Image src={on ? right_filled : right_empty}
            width={256}
            height={128}
            alt="Right indicator image" />
    );
}

export function LeftIndicator({ on }: { on: boolean }) {
    return (
        <Image src={on ? left_filled : left_empty}
            width={256}
            height={128}
            alt="Right indicator image." />
    );
}