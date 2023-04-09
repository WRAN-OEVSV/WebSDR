import { ComponentFixture, TestBed } from '@angular/core/testing';

import { CopyToClipboradTextWithBtnComponent } from './copy-to-clipborad-text-with-btn.component';

describe('CopyToClipboradTextWithBtnComponent', () => {
  let component: CopyToClipboradTextWithBtnComponent;
  let fixture: ComponentFixture<CopyToClipboradTextWithBtnComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ CopyToClipboradTextWithBtnComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(CopyToClipboradTextWithBtnComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
