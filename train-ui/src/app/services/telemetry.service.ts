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
    channel: String;
    // currentSpeed: Number = 0.1;

    private messageSource = new BehaviorSubject('default message');
    currentMessage = this.messageSource.asObservable();

    private speedSource = new BehaviorSubject(0.1);
    currentSpeed = this.speedSource.asObservable();

    constructor(pubnub: PubNubAngular) {
        this.channel = 'hst01';
        var _this = this;
        pubnub.init({
            publishKey: environment.PUBLISH_KEY,
            subscribeKey: environment.SUBSCRIBE_KEY,
        });

        pubnub.addListener({
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

        pubnub.subscribe({
            channels: [this.channel],
            triggerEvents: ['message'],
        });
    }

    telemetryData!: Telemetry;

    ngOnInit() {}

    getSpeed(): Observable<any> {
        return of(this.currentSpeed);
    }

    private handleMessage(message: any) {
        // this.currentSpeed = message.message[0].status.speed;
        // console.log('New current speed: ' + this.currentSpeed);
        console.log(`Message: ${JSON.stringify(message)}`);
        console.log(`Speed: ${message.message[0].status.speed}`);

        this.speedSource.next(message.message[0].status.speed);        

    }
}
