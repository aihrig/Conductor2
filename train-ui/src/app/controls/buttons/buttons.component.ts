import { Component, OnInit } from '@angular/core';
import { faAngleDoubleUp } from '@fortawesome/free-solid-svg-icons';
import { faAngleDoubleDown } from '@fortawesome/free-solid-svg-icons';
import { trigger, state, style, animate, transition } from '@angular/animations';

@Component({
    selector: 'app-buttons',
    templateUrl: './buttons.component.html',
    styleUrls: ['./buttons.component.scss'],
    animations: [

    ]
})
export class ButtonsComponent implements OnInit {
    constructor() {}

    ngOnInit(): void {}

    faAngleDoubleUp = faAngleDoubleUp;
    faAngleDoubleDown = faAngleDoubleDown;

    currentSpeed: number = 0;
    speedIncrement: number = 10;
    speedMax: number = 100;
    speedMin: number = 0;

    setState(evt: Event, button: String) {
        evt.preventDefault();

        switch (button.toLowerCase()) {
            case 'forward': {
                console.log('forward action');
                if (this.currentSpeed < this.speedMax) {
                    this.currentSpeed += this.speedIncrement;
                }
                break;
            }
            case 'reverse': {
                console.log('reverse action');
                if (this.currentSpeed > this.speedMin) {
                    this.currentSpeed -= this.speedIncrement;
                }
                break;
            }
        }

        console.log(this.currentSpeed);
    }
}
