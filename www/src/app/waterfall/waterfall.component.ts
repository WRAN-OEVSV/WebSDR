import {Component, ElementRef, Input, OnInit} from '@angular/core';
import {WebsocketService} from "../websocket.service";

// @ts-ignore
import {Spectrum} from './spectrum'

@Component({
  selector: 'app-waterfall',
  templateUrl: './waterfall.component.html',
  styleUrls: ['./waterfall.component.css']
})
export class WaterfallComponent implements OnInit {

  spectrum?: Spectrum;
  center: string = "";
  tx: string = "";
  min: string = "";
  max: string = "";
  @Input("keybindingEnabled")
  keybindingEnabled: boolean = false;

  constructor(private websocket: WebsocketService, private ref: ElementRef) {
  }

  ngOnInit(): void {
    const container = this.ref.nativeElement.querySelector('#containerWaterfall') as HTMLDivElement;
    const waterfall = this.ref.nativeElement.querySelector('#waterfall') as HTMLCanvasElement;
    waterfall.width = container.clientWidth;
    waterfall.height = container.clientHeight;
    this.spectrum = new Spectrum(
      "waterfall", {
        spectrumPercent: 25,
        logger: 'log',
      //autoScale: true
      });
    this.websocket.getEmitter().subscribe((data) => this.processEvent(data));


    window.addEventListener("keydown", (e) => {
      if (this.keybindingEnabled) {
        this.spectrum?.onKeypress(e);
      }
    });
  }


  private processEvent(data: any) {
    if (!this.spectrum) {
      return;
    }
    this.spectrum.setWebSocket(this.websocket.getSocket());
    if (data.s) {
      this.spectrum.addData(data.s);
      let num = parseFloat(data.center);
      let bw = parseFloat(data.span);
      let tx = parseFloat(data.txFreq);
      this.center = 'RX: ' + (num/1_000_000).toFixed(3) + ' MHz |';
      this.tx = ' TX: ' + (tx/1_000_000).toFixed(3) + ' MHz';
      this.min = ((num - bw/2)/1_000_000).toFixed(3) + ' MHz';
      this.max = ((num + bw/2)/1_000_000).toFixed(3) + ' MHz';
    } else {
      if (data.center) {
        this.spectrum.setCenterHz(data.center);

      }
      if (data.span) {
        this.spectrum.setSpanHz(data.span);
      }
      if (data.gain) {
        this.spectrum.setGain(data.gain);
      }
      if (data.framerate) {
        this.spectrum.setFps(data.framerate);
      }
      this.spectrum.log(" > Freq:" + data.center / 1_000_000 + " MHz | Span: " + data.span / 1_000_000 + " MHz | Gain: " + data.gain + "dB | Fps: " + data.framerate);
    }
  }
}
