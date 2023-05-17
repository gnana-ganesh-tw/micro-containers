import Head from 'next/head'
import styles from '@/styles/Home.module.css'
import Simulator from '@/components/simulator'
import React from 'react'
import RaspberryPiInterface from '@/components/raspberry_pi_interface'
import Esp32Interface from '@/components/esp32_interface'
import Pipeline from '@/components/pipeline'

export default function Home() {
  return (
    <>
      <Head>
        <title>Wasm Demo Application</title>
        <meta name="description" content="Software Defined Vehicles" />
        <meta name="viewport" content="width=device-width, initial-scale=1" />
        <link rel="icon" href="/favicon.ico" />
        <link href='https://fonts.googleapis.com/css?family=Inter' rel='stylesheet' />
      </Head>

      <div className={styles.top_info_bar}>
        <h2> WebAssembly Demo Application </h2>
        <p>
          The Wasm Demo App is a web application which provides a single user interface
          to build, deploy and run the traffic controller Wasm application binary on three
          different platforms - Web, Raspberry Pi and ESP32.
        </p>
      </div>

      <div className={styles.app_page}>
        <div className={styles.app_left_column}>
          <div className={`w3-card w3-margin-top w3-margin-left`}>
            <RaspberryPiInterface />
          </div>

          <div className={`w3-card w3-margin-top w3-margin-left`}>
            <Esp32Interface />
          </div>
        </div>

        <div className={styles.app_right_column}>
          <div className={`w3-card w3-margin-top w3-margin-left w3-margin-right`}>
            <div className={styles.card_heading}>
              Web Simulator: Runs Wasm in the browser
            </div>
            <div className={styles.web_simulator}>
              <Simulator right={false} left={false} />
            </div>
          </div>

          <div className={`w3-card w3-margin-top w3-margin-left w3-margin-right`}>
            <Pipeline />
          </div>

        </div>
      </div >
    </>
  )
}
