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
    // currentSpeed: Number = 0.1;

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
                _this.handleMessage(message);
            },
        });

        this.pubnub.subscribe({
            channels: [this.channel],
            triggerEvents: ['message'],
        });
    }

    telemetryData!: Telemetry;


    ngOnInit() {
    }

    updateStatus() {
        console.log('updateStatus() fired');
        // Fire off a request to the train for status
        this.pubnub.publish(
            {
                message: [{
                    command: 'getstatus',
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

    getSpeed(): Observable<any> {
        return of(this.currentSpeed);
    }

    setSpeed(speed: number): void {
        console.log(`Speed: ${speed}`);
        if (speed === 0) {
            this.speedSource.next(0.1);
        } else {
            this.speedSource.next(speed);
        }
    }

    private handleMessage(message: any) {
        if (typeof message.message === 'object') {
            let msg = message.message;
            console.log(`message.message: ${JSON.stringify(message.message)}`);
            if (msg.status) {
                // message.message "{status: {speed:0, headlightsOn:0} }"
                console.log(
                    `message.message.status: ${JSON.stringify(
                        message.message.status
                    )}`
                );

                if (msg.status.speed) {
                    this.setSpeed(Number(msg.status.speed));
                }
            }
        }
    }
}
