import { Injectable, OnInit } from '@angular/core';
import { PubNubAngular } from 'pubnub-angular2';
import { BehaviorSubject, Observable, of } from 'rxjs';
import { environment } from 'src/environments/environment';
import { Telemetry } from '../interfaces/telemetry';

@Injectable({
    providedIn: 'root',
})
export class TelemetryService implements OnInit {
    pubnub!: PubNubAngular;
    channel: string;
    lastSpeed: String = '0.1';

    private speedSource = new BehaviorSubject(0.1);
    currentSpeed = this.speedSource.asObservable();

    constructor(pubnub: PubNubAngular) {
        this.channel = environment.CHANNEL;
        this.pubnub = pubnub;
        var _this = this;

        this.pubnub.init({
            publishKey: environment.PUBLISH_KEY,
            subscribeKey: environment.SUBSCRIBE_KEY,
        });

        this.pubnub.addListener({
            status: function (st: {
                category: string;
                errorData: { message: any };
            }) {
                if (st.category === 'PNUnknownCategory') {
                    var newState = {
                        new: 'error',
                    };
                    pubnub.setState(
                        {
                            state: newState,
                        },
                        function (status) {
                            console.log(st.errorData.message);
                        }
                    );
                }
            },
            message: function (message: any) {
                console.log(
                    `#DEBUG# telemetry.service incoming message -> ${JSON.stringify(
                        message
                    )}`
                );
                if (message.message.status) _this.handleMessage(message);
            },
        });

        this.pubnub.subscribe({
            channels: [this.channel],
            triggerEvents: ['message', 'status'],
        });
    }

    ngOnInit() {}

    updateStatus() {
        console.log('#DEBUG# updateStatus() fired');
        // Fire off a request to the train for status
        this.pubnub.publish(
            {
                message: [
                    {
                        command: 'getstatus',
                    },
                ],
                channel: this.channel,
            },
            function (status, response) {
                if (status.error) {
                    // handle error
                    console.log(`#DEBUG# ${status}`);
                } else {
                    console.log(
                        `#DEBUG# message Published w/ timetoken ${response.timetoken}`
                    );
                }
            }
        );
    }

    getSpeed(): String {
        return this.lastSpeed;
    }

    increaseSpeed(amount: number) {
        this.setSpeed(Number(this.lastSpeed) + amount);
    }

    decreaseSpeed(amount: number) {
        this.setSpeed(Number(this.lastSpeed) - amount);
    }

    setSpeed(speed: number): void {
        // Limit speed range to 0 - 100
        speed = speed >= 100 ? 100 : speed;
        speed = speed <= 0 ? 0.1 : speed;
        console.log(`#DEBUG# setSpeed() -> Speed: ${speed}`);

        if (speed === 0) {
            this.speedSource.next(0.1);
            this.lastSpeed = '0.1';
        } else {
            this.speedSource.next(speed);
            this.lastSpeed = String(speed);
        }

        // TODO: Handle direction
        this.pubnub.publish(
            {
                message: [
                    {
                        command: 'drive',
                        direction: 'forward',
                        speed: speed,
                    },
                ],
                channel: this.channel,
            },
            function (status, response) {
                if (status.error) {
                    // handle error
                    console.log(`#DEBUG# status`);
                } else {
                    console.log(
                        `message Published w/ timetoken ${response.timetoken}`
                    );
                }
            }
        );
    }

    private handleMessage(message: any) {
        console.log(
            `#DEBUG# telemetry.service handleMessage() called with -> ${JSON.stringify(
                message
            )}`
        );
        if (typeof message.message === 'object') {
            let msg = message.message;
            if (msg.hasOwnProperty('status')) {
                if (msg.status.hasOwnProperty('speed')) {
                    console.log(`#DEBUG# SPEED -------------->>> ${msg.status.speed}`);
                    // this.setSpeed(msg.status.speed);
                    let speed = msg.status.speed;
                    if (speed == 0) {
                        speed = 0.1;
                    }
                    this.speedSource.next(speed);
                    this.lastSpeed = String(speed);
                }
            }
        }
    }
}
