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
    lastSpeed!: String;

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
                if (message.message.status)
                    _this.handleMessage(message);
            },
        });

        this.pubnub.subscribe({
            channels: [this.channel],
            triggerEvents: ['message'],
        });
    }

    ngOnInit() {}

    updateStatus() {
        console.log('updateStatus() fired');
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
                    console.log(status);
                } else {
                    console.log(
                        'message Published w/ timetoken',
                        response.timetoken
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
        console.log(`setSpeed() -> Speed: ${speed}`);
        if (speed === 0) {
            this.speedSource.next(0.1);
            this.lastSpeed = "0.1";
        } else {
            this.speedSource.next(speed);
            this.lastSpeed = String(speed);
        }

        // TODO: Handle direction
        this.pubnub.publish(
            {
                message: [{
                    command: 'drive',
                    direction: 'forward',
                    speed: speed
                }],
                channel: this.channel,
            },
            function (status, response) {
                if (status.error) {
                    // handle error
                    console.log(status);
                } else {
                    console.log(
                        'message Published w/ timetoken',
                        response.timetoken
                    );
                }
            }
        );

    }

    private handleMessage(message: any) {

        // this.currentSpeed.subscribe(value => {
        //     console.log(
        //         'telemetry.service -> handleMessage() : currentSpeed: ' + value
        //     );
        // });
        if (typeof message.message === 'object') {
            let msg = message.message;
            // console.log(`message.message: ${JSON.stringify(msg)}`);
            if (msg.status.speed) {
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
