import {Component, Input} from '@angular/core';
import {Clipboard} from "@angular/cdk/clipboard";

@Component({
  selector: 'app-copy-to-clipborad-text-with-btn',
  templateUrl: './copy-to-clipborad-text-with-btn.component.html',
  styleUrls: ['./copy-to-clipborad-text-with-btn.component.css']
})
export class CopyToClipboradTextWithBtnComponent {

  @Input("text") text: string;
  constructor(private clipBoard: Clipboard) {
    this.text = '';
  }


  copyToClipBoard() {
    this.clipBoard.copy(this.text);
  }
}
