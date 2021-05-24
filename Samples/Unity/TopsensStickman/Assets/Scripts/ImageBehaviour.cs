using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;
using UnityEngine.UI;
using Topsens;

using Vector2 = UnityEngine.Vector2;
using Vector3 = UnityEngine.Vector3;

public class ImageBehaviour : MonoBehaviour
{
    void Start()
    {
        var palette = new Color[0x1000];

        for (uint d = 0; d < 0x190; d++)
        {
            palette[d] = new Color(0.0f, 0.0f, 0.0f, 1.0f);
        }

        for (uint d = 0x190; d < 0x400; d++)
        {
            var r = (0xFF - 0xFF * (d - 0x190) / (0x400 - 0x190)) / 255.0f;
            palette[d] = new Color(r, 0.0f, 1.0f, 1.0f);
        }

        for (uint d = 0x400; d < 0x600; d++)
        {
            var g = (0xFF * (d - 0x400) / (0x600 - 0x400)) / 255.0f;
            palette[d] = new Color(0.0f, g, 1.0f, 1.0f);
        }

        for (uint d = 0x600; d < 0x800; d++)
        {
            var b = (0xFF - 0xFF * (d - 0x600) / (0x800 - 0x600)) / 255.0f;
            palette[d] = new Color(0.0f, 1.0f, b, 1.0f);
        }

        for (uint d = 0x800; d < 0xC00; d++)
        {
            var r = (0xFF * (d - 0x800) / (0xC00 - 0x800)) / 255.0f;
            palette[d] = new Color(r, 1.0f, 0.0f, 1.0f);
        }

        for (uint d = 0xC00; d < 0x1000; d++)
        {
            var g = (0xFF - 0xFF * (d - 0xC00) / (0x1000 - 0xC00)) / 255.0f;
            palette[d] = new Color(1.0f, g, 0.0f, 1.0f);
        }

        this.palette = palette;
    }

    void Update()
    {
        if (this.updated)
        {
            this.updated = false;

            this.Arrange();

            if (null == this.texture)
            {
                this.texture = new Texture2D(SensorBehaviour.Resolution.Width(), SensorBehaviour.Resolution.Height());

                var image = this.gameObject.GetComponent<RawImage>();
                image.texture = this.texture;
                image.color = Color.white;
            }

            if (null != this.pixels)
            {
                lock (this.pixels)
                {
                    this.texture.SetPixels(this.pixels);
                    this.texture.Apply();
                }
            }
        }
    }

    void OnEnable()
    {
        if (null != Camera.main)
        {
            this.sensor = Camera.main.GetComponent<SensorBehaviour>();
            if (null != this.sensor)
            {
                this.sensor.DepthReady += this.OnDepth;
            }
        }
    }

    void OnDisable()
    {
        if (null != this.sensor)
        {
            this.sensor.DepthReady -= this.OnDepth;
            this.sensor = null;
        }
    }

    private void Arrange()
    {
        var w = Screen.width * 0.2f;
        var h = w * SensorBehaviour.Resolution.Height() / SensorBehaviour.Resolution.Width();

        Vector3 position = new Vector3();

        switch (this.orientation)
        {
            case Orientation.Landscape:
            {
                position = new Vector3(Screen.width * 0.45f - w * 0.5f, -Screen.height * 0.45f + h * 0.5f, 0.0f);
                this.gameObject.transform.rotation = Quaternion.identity;
                break;
            }

            case Orientation.PortraitClockwise:
            {
                position = new Vector3(Screen.width * 0.45f - h * 0.5f, -Screen.height * 0.45f + w * 0.5f, 0.0f);
                this.gameObject.transform.rotation = Quaternion.AngleAxis(90.0f, Vector3.back);
                break;
            }

            case Orientation.PortraitAntiClockwise:
            {
                position = new Vector3(Screen.width * 0.45f - h * 0.5f, -Screen.height * 0.45f + w * 0.5f, 0.0f);
                this.gameObject.transform.rotation = Quaternion.AngleAxis(90.0f, Vector3.forward);
                break;
            }

            default: break;
        }

        #if !UNITY_EDITOR
        var rect = this.gameObject.GetComponent<RectTransform>();
        rect.sizeDelta = new Vector2(w, h);
        rect.localPosition = position;
        #endif
    }

    private void OnDepth(DepthFrame frame, Orientation orientation)
    {
        if (null == this.palette)
        {
            return;
        }

        if (null == this.pixels || this.pixels.Length != frame.Pixels.Length)
        {
            this.pixels = new Color[frame.Pixels.Length];
        }

        lock (this.pixels)
        {
            this.orientation = orientation;

            for (int i = 0; i < frame.Pixels.Length; i++)
            {
                var d = frame.Pixels[i];
                this.pixels[i] = d < this.palette.Length ? this.palette[d] : Color.red;
            }
        }

        this.updated = true;
    }

    bool updated;
    Color[] pixels;
    Color[] palette;
    Texture2D texture;
    Orientation orientation;
    SensorBehaviour sensor;
}