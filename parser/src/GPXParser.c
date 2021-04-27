
#include "GPXParser.h"

GPXData *dt;
Waypoint *wp;
Route *rt;
Waypoint *rtpt;
Track *tk;
TrackSegment *tksg;
Waypoint *tpt;

int gpxdata_cnt = 0;

// Gpx Data List Helpers

void deleteGpxData(void *data)
{
    if (data == NULL)
    {
        return;
    }
    GPXData *d = (GPXData *)data;
    free(d);
}

char *gpxDataToString(void *data)
{
    if (data == NULL)
    {
        return "";
    }
    char *str = (char *)malloc(100000);
    sprintf(str, "%s : %s\n", ((GPXData *)data)->name, ((GPXData *)data)->value);
    return str;
}

int compareGpxData(const void *first, const void *second)
{
    if (!first || !second)
        return 0;

    GPXData *f = (GPXData *)first;
    GPXData *s = (GPXData *)second;
    if ((strcmp(f->name, s->name) != 0) || (strcmp(f->value, s->value) != 0))
        return 0;

    return 1;
}

// Waypoint List Helpers

void deleteWaypoint(void *data)
{
    if (data == NULL)
    {
        return;
    }
    free(((Waypoint *)data)->name);
    freeList(((Waypoint *)data)->otherData);
    free(data);
}

char *waypointToString(void *data)
{
    if (data == NULL)
    {
        return "";
    }
    Waypoint *w = (Waypoint *)data;
    char *str = (char *)malloc(100000);

    sprintf(str, "Waypoint - \nName: %s\nLatitude: %f\nLongitude: %f\n", w->name, w->latitude, w->longitude);

    ListIterator itr = createIterator(w->otherData);
    GPXData *el;
    while ((el = nextElement(&itr)) != NULL)
    {
        char *curr = w->otherData->printData(el);
        strcat(str, curr);
        free(curr);
    }
    return str;
}

int compareWaypoints(const void *first, const void *second)
{
    if (!first || !second)
        return 0;

    Waypoint *w1 = ((Waypoint *)first);
    Waypoint *w2 = ((Waypoint *)second);

    if ((strcmp(w1->name, w2->name) != 0))
    {
        return 0;
    }
    if ((w1->latitude != w2->latitude))
    {
        return 0;
    }
    if ((w1->longitude != w2->longitude))
    {
        return 0;
    }
    ListIterator itr1 = createIterator(w1->otherData);
    ListIterator itr2 = createIterator(w2->otherData);

    if (w1->otherData->length != w2->otherData->length)
    {
        return 0;
    }

    int n = w1->otherData->length;

    int check_w1[n];
    memset(check_w1, 0, sizeof(check_w1));
    int check_w2[n];
    memset(check_w2, 0, sizeof(check_w2));

    int i1 = 0;

    GPXData *el1;
    GPXData *el2;

    while ((el1 = nextElement(&itr1)) != NULL)
    {
        int i2 = 0;
        itr2 = createIterator(w2->otherData);
        while ((el2 = nextElement(&itr2)) != NULL)
        {
            if (compareGpxData(el1, el2))
            {
                check_w1[i1] = 1;
                check_w2[i2] = 1;
                break;
            }
            i2++;
        }
        i1++;
    }
    for (int i = 0; i < n; i++)
    {
        if ((!check_w1[i]) || (!check_w2[i]))
        {
            return 0;
        }
    }
    return 1;
}

// Route List Helpers
void deleteRoute(void *data)
{
    if (data == NULL)
        return;

    free(((Route *)data)->name);
    freeList(((Route *)data)->waypoints);
    freeList(((Route *)data)->otherData);
    free(data);
}

char *routeToString(void *data)
{
    if (data == NULL)
    {
        return "";
    }

    Route *r = ((Route *)data);
    char *str = (char *)malloc(1000000);
    sprintf(str, "Route - \nName: %s\n\n", r->name);

    // printf("%d %d\n", r->waypoints->length, r->otherData->length);

    ListIterator itr = createIterator(r->waypoints);
    Waypoint *el;
    while ((el = nextElement(&itr)) != NULL)
    {
        char *curr = r->waypoints->printData(el);
        strcat(str, curr);
        free(curr);
    }
    if (r->otherData->length > 0)
        strcat(str, "\nRoute's Other Data - \n");

    GPXData *el1;
    ListIterator itr1 = createIterator(r->otherData);
    while ((el1 = nextElement(&itr1)) != NULL)
    {
        char *curr = r->otherData->printData(el1);
        strcat(str, curr);
        free(curr);
    }
    return str;
}

int compareRoutes(const void *first, const void *second)
{
    if (!first || !second)
        return 0;

    Route *f = ((Route *)first);
    Route *s = ((Route *)second);

    if (strcmp(f->name, s->name) != 0)
        return 0;

    if (f->waypoints->length != s->waypoints->length)
        return 0;

    if (f->otherData->length != s->otherData->length)
        return 0;

    int n = f->otherData->length;

    int check_f[n];
    memset(check_f, 0, sizeof(check_f));
    int check_s[n];
    memset(check_s, 0, sizeof(check_s));

    int i1 = 0;

    GPXData *el01;
    GPXData *el02;

    ListIterator itr01 = createIterator(f->otherData);
    ListIterator itr02 = createIterator(s->otherData);

    while ((el01 = nextElement(&itr01)) != NULL)
    {
        int i2 = 0;
        itr02 = createIterator(s->otherData);
        while ((el02 = nextElement(&itr02)) != NULL)
        {
            if (compareGpxData(el01, el02))
            {
                // printf("%d %d\n\n",i1,i2);
                check_f[i1] = 1;
                check_s[i2] = 1;
                break;
            }
            i2++;
        }
        i1++;
    }

    for (int i = 0; i < n; i++)
    {
        if ((!check_f[i]) || (!check_s[i]))
        {
            return 0;
        }
    }

    Waypoint *el11;
    Waypoint *el12;

    n = f->waypoints->length;
    int check_f1[n];
    memset(check_f1, 0, sizeof(check_f1));
    int check_s1[n];
    memset(check_s1, 0, sizeof(check_s1));

    i1 = 0;

    ListIterator itr11 = createIterator(f->waypoints);
    ListIterator itr12 = createIterator(s->waypoints);

    while ((el11 = nextElement(&itr11)) != NULL)
    {
        int i2 = 0;
        itr12 = createIterator(s->waypoints);
        while ((el12 = nextElement(&itr12)) != NULL)
        {
            if (compareWaypoints(el11, el12))
            {
                check_f1[i1] = 1;
                check_s1[i2] = 1;
                break;
            }
            i2++;
        }
        i1++;
    }
    for (int i = 0; i < n; i++)
    {
        if ((!check_f1[i]) || (!check_s1[i]))
        {
            return 0;
        }
    }

    return 1;
}

// TrackSegment List Helpers

void deleteTrackSegment(void *data)
{
    if (data == NULL)
        return;
    freeList(((TrackSegment *)data)->waypoints);
    free(data);
}

char *trackSegmentToString(void *data)
{
    if (data == NULL)
    {
        return "";
    }

    char *str = (char *)malloc(100000);
    strcpy(str, "");
    strcat(str, "Track Segment - \n\n");

    ListIterator itr = createIterator(((TrackSegment *)data)->waypoints);
    Waypoint *el;
    while ((el = nextElement(&itr)) != NULL)
    {
        char *curr = ((TrackSegment *)data)->waypoints->printData(el);
        strcat(str, curr);
        free(curr);
    }
    return str;
}
int compareTrackSegments(const void *first, const void *second)
{
    if (!first || !second)
        return 0;

    TrackSegment *f = ((TrackSegment *)first);
    TrackSegment *s = ((TrackSegment *)second);

    if (f->waypoints->length != s->waypoints->length)
        return 0;

    int n = f->waypoints->length;

    int check_f[n];
    memset(check_f, 0, sizeof(check_f));
    int check_s[n];
    memset(check_s, 0, sizeof(check_s));

    int i1 = 0;

    Waypoint *el11;
    Waypoint *el12;

    ListIterator itr11 = createIterator(f->waypoints);
    ListIterator itr12 = createIterator(s->waypoints);

    while ((el11 = nextElement(&itr11)) != NULL)
    {
        int i2 = 0;
        itr12 = createIterator(s->waypoints);
        while ((el12 = nextElement(&itr12)) != NULL)
        {
            if (compareWaypoints(el11, el12))
            {
                check_f[i1] = 1;
                check_s[i2] = 1;
                break;
            }
            i2++;
        }
        i1++;
    }

    for (int i = 0; i < n; i++)
    {
        if ((!check_f[i]) || (!check_s[i]))
        {
            return 0;
        }
    }

    return 1;
}

// Track List Helper Functions

void deleteTrack(void *data)
{
    if (data == NULL)
        return;

    free(((Track *)data)->name);
    freeList(((Track *)data)->segments);
    freeList(((Track *)data)->otherData);
    free(data);
}

char *trackToString(void *data)
{
    if (data == NULL)
    {
        return "";
    }
    Track *t = ((Track *)data);
    char *str = (char *)malloc(1000000);
    strcpy(str, "");
    sprintf(str, "Track - \nName: %s\n\n", t->name);

    ListIterator itr = createIterator(t->segments);
    TrackSegment *el;

    while ((el = nextElement(&itr)) != NULL)
    {
        char *curr = t->segments->printData(el);
        strcat(str, curr);
        free(curr);
    }

    if (t->otherData->length > 0)
        strcat(str, "\nTrack's Other Data - \n");

    ListIterator itr1 = createIterator(t->otherData);
    GPXData *el1;
    while ((el1 = nextElement(&itr1)) != NULL)
    {
        char *curr = t->otherData->printData(el1);
        strcat(str, curr);
        free(curr);
    }

    return str;
}

int compareTracks(const void *first, const void *second)
{
    if (!first || !second)
        return 0;

    Track *f = ((Track *)first);
    Track *s = ((Track *)second);

    if (strcmp(f->name, s->name) != 0)
        return 0;

    int n = f->otherData->length;

    int check_f[n];
    memset(check_f, 0, sizeof(check_f));
    int check_s[n];
    memset(check_s, 0, sizeof(check_s));

    int i1 = 0;

    GPXData *el01;
    GPXData *el02;

    ListIterator itr01 = createIterator(f->otherData);
    ListIterator itr02 = createIterator(s->otherData);

    while ((el01 = nextElement(&itr01)) != NULL)
    {
        int i2 = 0;
        itr02 = createIterator(s->otherData);
        while ((el02 = nextElement(&itr02)) != NULL)
        {
            if (compareGpxData(el01, el02))
            {
                check_f[i1] = 1;
                check_s[i2] = 1;
                break;
            }
            i2++;
        }
        i1++;
    }

    for (int i = 0; i < n; i++)
    {
        if ((!check_f[i]) || (!check_s[i]))
        {
            return 0;
        }
    }

    TrackSegment *el11;
    TrackSegment *el12;

    n = f->segments->length;

    int check_f1[n];
    memset(check_f1, 0, sizeof(check_f1));
    int check_s1[n];
    memset(check_s1, 0, sizeof(check_s1));

    i1 = 0;

    ListIterator itr11 = createIterator(f->segments);
    ListIterator itr12 = createIterator(s->segments);

    while ((el11 = nextElement(&itr11)) != NULL)
    {
        int i2 = 0;
        itr12 = createIterator(s->segments);
        while ((el12 = nextElement(&itr12)) != NULL)
        {
            if (compareTrackSegments(el11, el12))
            {
                check_f1[i1] = 1;
                check_s1[i2] = 1;
                break;
            }
            i2++;
        }
        i1++;
    }

    for (int i = 0; i < n; i++)
    {
        if ((!check_f1[i]) || (!check_s1[i]))
        {
            return 0;
        }
    }

    return 1;
}

// List Helpers Done

char *GPXdocToString(GPXdoc *doc)
{
    if (doc == NULL)
    {
        return "";
    }
    char *str = (char *)malloc(10000000);
    sprintf(str, "GPX Document - \n\tNamespace: %s\n\tVersion: %.1f\n\tCreator: %s\n\n", doc->namespace, doc->version, doc->creator);

    char *temp = (char *)malloc(100000);
    if (doc->waypoints->length != 0)
    {
        strcat(str, "\t\tWaypoints -->\n");
        ListIterator itr = createIterator(doc->waypoints);
        Waypoint *el;
        int i = 1;
        while ((el = nextElement(&itr)) != NULL)
        {
            sprintf(temp, "%d. ", i);
            strcat(str, temp);
            char *curr = doc->waypoints->printData(el);
            strcat(str, curr);
            i++;
            free(curr);
        }
        strcat(str, "\n");
    }
    if (doc->routes->length != 0)
    {
        strcat(str, "\t\tRoutes -->\n");
        ListIterator itr = createIterator(doc->routes);
        Route *el;
        int i = 1;
        while ((el = nextElement(&itr)) != NULL)
        {
            sprintf(temp, "%d. ", i);
            strcat(str, temp);
            char *curr = doc->routes->printData(el);
            strcat(str, curr);
            i++;
            free(curr);
        }
        strcat(str, "\n");
    }
    if (doc->tracks->length != 0)
    {
        strcat(str, "\t\tTrack -->\n");
        ListIterator itr = createIterator(doc->tracks);
        Track *el;
        int i = 1;
        while ((el = nextElement(&itr)) != NULL)
        {
            sprintf(temp, "%d. ", i);
            strcat(str, temp);
            char *curr = doc->tracks->printData(el);
            strcat(str, curr);
            i++;
            free(curr);
        }
    }
    free(temp);
    return str;
}

void parser(xmlDoc *doc, xmlNode *node, GPXdoc **gpx)
{
    while (node)
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            if ((strcmp(((char *)(node->name)), "wpt") == 0))
            {
                wp = (Waypoint *)malloc(sizeof(Waypoint));
                wp->name = (char *)malloc(sizeof(char) * 10000);
                strcpy(wp->name, "");
                wp->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData);

                xmlAttr *attr;
                for (attr = node->properties; attr != NULL; attr = attr->next)
                {
                    xmlNode *value = attr->children;
                    char *attrName = (char *)attr->name;
                    char *cont = (char *)(value->content);

                    if (strcmp(attrName, "lat") == 0)
                    {
                        char *ptr;
                        double l;
                        char *temp = (char *)malloc(100000);
                        strcpy(temp, cont);
                        l = strtod(temp, &ptr);
                        wp->latitude = l;
                        free(temp);
                    }
                    else if (strcmp(attrName, "lon") == 0)
                    {
                        char *ptr;
                        double l;
                        char *temp = (char *)malloc(100000);
                        strcpy(temp, cont);
                        l = strtod(temp, &ptr);
                        wp->longitude = l;
                        free(temp);
                    }
                    else if (strcmp(attrName, "name") == 0)
                    {
                        strcpy(wp->name, cont);
                    }
                }
            }
            else if ((strcmp(((char *)(node->name)), "trkpt") == 0))
            {
                tpt = (Waypoint *)malloc(sizeof(Waypoint));
                tpt->name = (char *)malloc(sizeof(char) * 100000);
                strcpy(tpt->name, "");
                tpt->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData);

                xmlAttr *attr;
                for (attr = node->properties; attr != NULL; attr = attr->next)
                {
                    xmlNode *value = attr->children;
                    char *attrName = (char *)attr->name;
                    char *cont = (char *)(value->content);

                    if (strcmp(attrName, "lat") == 0)
                    {
                        char *ptr;
                        double l;
                        char *temp = (char *)malloc(100000);
                        strcpy(temp, cont);
                        l = strtod(temp, &ptr);
                        tpt->latitude = l;
                        free(temp);
                    }
                    else if (strcmp(attrName, "lon") == 0)
                    {
                        char *ptr;
                        double l;
                        char *temp = (char *)malloc(100000);
                        strcpy(temp, cont);
                        l = strtod(temp, &ptr);
                        tpt->longitude = l;
                        free(temp);
                    }
                    else if (strcmp(attrName, "name") == 0)
                    {
                        strcpy(tpt->name, cont);
                    }
                }
            }
            else if ((strcmp(((char *)(node->name)), "rtept") == 0))
            {
                rtpt = (Waypoint *)malloc(sizeof(Waypoint));
                rtpt->name = (char *)malloc(sizeof(char) * 100000);
                strcpy(rtpt->name, "");
                rtpt->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData);

                xmlAttr *attr;
                for (attr = node->properties; attr != NULL; attr = attr->next)
                {
                    xmlNode *value = attr->children;
                    char *attrName = (char *)attr->name;
                    char *cont = (char *)(value->content);

                    if (strcmp(attrName, "lat") == 0)
                    {
                        char *ptr;
                        double l;
                        char *temp = (char *)malloc(100000);
                        strcpy(temp, cont);
                        l = strtod(temp, &ptr);
                        rtpt->latitude = l;
                        free(temp);
                    }
                    else if (strcmp(attrName, "lon") == 0)
                    {
                        char *ptr;
                        double l;
                        char *temp = (char *)malloc(100000);
                        strcpy(temp, cont);
                        l = strtod(temp, &ptr);
                        rtpt->longitude = l;
                        free(temp);
                    }
                    else if (strcmp(attrName, "name") == 0)
                    {
                        strcpy(rtpt->name, cont);
                    }
                }
            }
            else if (strcmp(((char *)(node->name)), "trk") == 0) // Track
            {
                tk = (Track *)malloc(sizeof(Track));
                tk->name = (char *)malloc(100000);
                strcpy(tk->name, "");
                tk->segments = initializeList(trackSegmentToString, deleteTrackSegment, compareTrackSegments);
                tk->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData);
            }
            else if (strcmp(((char *)(node->name)), "trkseg") == 0) // TrackSegment
            {
                tksg = (TrackSegment *)malloc(sizeof(TrackSegment));
                tksg->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
            }
            else if (strcmp(((char *)(node->name)), "rte") == 0)
            {
                rt = (Route *)malloc(sizeof(Route));
                rt->name = (char *)malloc(100000);
                strcpy(rt->name, "");
                rt->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
                rt->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData);

                xmlAttr *attr;
                for (attr = node->properties; attr != NULL; attr = attr->next)
                {
                    xmlNode *value = attr->children;
                    char *attrName = (char *)attr->name;
                    char *cont = (char *)(value->content);
                    if (strcmp(attrName, "name") == 0)
                    {
                        strcpy(rt->name, cont);
                    }
                }
            }
            else
            {
                dt = malloc(sizeof(GPXData) + 100000 * sizeof(char));
                strcpy(dt->name, ((char *)(node->name)));
                char *ss = ((char *)(xmlNodeGetContent(node)));
                strcpy(dt->value, ss);
                free(ss);
                if (!((strcmp(dt->name, "name") == 0) && (strcmp(dt->value, "") == 0)))
                    gpxdata_cnt++;
            }
        }

        parser(doc, node->children, gpx);

        if (node->type == XML_ELEMENT_NODE)
        {

            if (strcmp(((char *)(node->parent->name)), "wpt") == 0)
            {
                if (strcmp(dt->name, "name") == 0)
                {
                    strcpy(wp->name, dt->value);
                    deleteGpxData(dt);
                }
                else
                {
                    insertBack(wp->otherData, dt);
                }
                dt = NULL;
                goto nxt;
            }

            else if (strcmp(((char *)(node->parent->name)), "rte") == 0)
            {
                if (dt)
                {
                    if (strcmp(dt->name, "name") == 0)
                    {
                        strcpy(rt->name, dt->value);
                        deleteGpxData(dt);
                    }
                    else if (strcmp(dt->name, "name") != 0)
                    {
                        insertBack(rt->otherData, dt);
                    }
                    dt = NULL;
                }
                if (rtpt)
                {
                    insertBack(rt->waypoints, rtpt);
                    rtpt = NULL;
                }
                goto nxt;
            }

            else if (strcmp(((char *)(node->parent->name)), "rtept") == 0)
            {
                if (strcmp(dt->name, "name") == 0)
                {
                    strcpy(rtpt->name, dt->value);
                    deleteGpxData(dt);
                }
                else if (strcmp(dt->name, "name") != 0)
                {
                    insertBack(rtpt->otherData, dt);
                }
                dt = NULL;
                goto nxt;
            }

            else if (strcmp(((char *)(node->parent->name)), "trk") == 0)
            {
                if (dt != NULL)
                {
                    if (strcmp(dt->name, "name") == 0)
                    {
                        strcpy(tk->name, dt->value);
                        deleteGpxData(dt);
                    }
                    else
                    {
                        insertBack(tk->otherData, dt);
                    }
                    dt = NULL;
                }
                if (tksg != NULL)
                {
                    insertBack(tk->segments, tksg);
                    tksg = NULL;
                }
                goto nxt;
            }

            else if (strcmp(((char *)(node->parent->name)), "trkpt") == 0)
            {
                if (strcmp(dt->name, "name") == 0)
                {
                    strcpy(tpt->name, dt->value);
                    deleteGpxData(dt);
                }
                else if (strcmp(dt->name, "name") != 0)
                {
                    insertBack(tpt->otherData, dt);
                }
                dt = NULL;
                goto nxt;
            }
            else if (strcmp(((char *)(node->parent->name)), "trkseg") == 0)
            {
                if (tpt)
                {
                    insertBack(tksg->waypoints, tpt);
                    tpt = NULL;
                }
                goto nxt;
            }

            else if (strcmp(((char *)node->name), "wpt") == 0)
            {
                insertBack((*gpx)->waypoints, wp);
                wp = NULL;
            }
            else if (strcmp(((char *)node->name), "rte") == 0)
            {
                insertBack((*gpx)->routes, rt);
                rt = NULL;
            }
            else if (strcmp(((char *)node->name), "trk") == 0)
            {
                insertBack((*gpx)->tracks, tk);
                tk = NULL;
            }
        }
    nxt:
        node = node->next;
    }
}

// FUNCTIONS :

int getNumWaypoints(const GPXdoc *doc)
{
    if (!doc)
        return 0;
    return doc->waypoints->length;
}

int getNumRoutes(const GPXdoc *doc)
{
    if (!doc)
        return 0;
    return doc->routes->length;
}

int getNumTracks(const GPXdoc *doc)
{
    if (!doc)
        return 0;
    return doc->tracks->length;
}

int getNumSegments(const GPXdoc *doc)
{
    if (!doc)
        return 0;
    int count = 0;
    if (doc->tracks->length != 0)
    {
        ListIterator itr = createIterator(doc->tracks);
        Track *el;
        while ((el = nextElement(&itr)) != NULL)
        {
            count += el->segments->length;
        }
    }
    return count;
}

int getNumGPXData(const GPXdoc *doc)
{
    if (!doc)
        return 0;
    return gpxdata_cnt;
}

Waypoint *getWaypoint(const GPXdoc *doc, char *name)
{
    if (!doc || !name)
    {
        return NULL;
    }
    if (doc->waypoints->length != 0)
    {
        ListIterator itr = createIterator(doc->waypoints);
        Waypoint *el;
        while ((el = nextElement(&itr)) != NULL)
        {
            if (strcmp(el->name, name) == 0)
                return el;
        }
    }
    return NULL;
}

Track *getTrack(const GPXdoc *doc, char *name)
{
    if (!doc || !name)
    {
        return NULL;
    }
    if (doc->tracks->length != 0)
    {
        ListIterator itr = createIterator(doc->tracks);
        Track *el;
        while ((el = nextElement(&itr)) != NULL)
        {
            if (strcmp(el->name, name) == 0)
                return el;
        }
    }
    return NULL;
}

Route *getRoute(const GPXdoc *doc, char *name)
{
    if (!doc || !name)
    {
        return NULL;
    }
    if (doc->routes->length != 0)
    {
        ListIterator itr = createIterator(doc->routes);
        Route *el;
        while ((el = nextElement(&itr)) != NULL)
        {
            if (strcmp(el->name, name) == 0)
                return el;
        }
    }
    return NULL;
}

GPXdoc *createGPXdoc(char *fileName)
{
    if ((fileName == NULL) || (strcmp(fileName, "") == 0))
    {
        return NULL;
    }

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL)
    {
        xmlCleanupParser();
        return NULL;
    }

    GPXdoc *gpx = (GPXdoc *)malloc(sizeof(GPXdoc));

    root_element = xmlDocGetRootElement(doc);

    xmlNsPtr *nslistt;
    nslistt = xmlGetNsList(doc, root_element);

    if (nslistt == NULL)
    {
        strcpy(gpx->namespace, "");
    }
    else
    {
        int i = 0;
        while ((*(nslistt + i)) != NULL)
        {
            xmlNsPtr curr = (*(nslistt + i));
            if (curr->prefix == NULL)
            {
                strcpy(gpx->namespace, ((char *)(curr->href)));
                break;
            }
            i++;
        }
    }

    free(nslistt);

    // VERsion Handled
    char *ss1 = (char *)(xmlGetProp(root_element, ((unsigned char *)("version"))));

    if (ss1 == NULL)
        gpx->version = 1.1;
    else
    {
        char *ptr;
        double ver;
        char *temp = (char *)malloc(100000);
        strcpy(temp, ss1);
        ver = strtod(temp, &ptr);
        gpx->version = ver;
        free(temp);
    }
    free(ss1);

    //creator
    gpx->creator = (char *)malloc(100000);
    char *ss = (char *)(xmlGetProp(root_element, ((unsigned char *)("creator"))));
    if (ss == NULL)
        strcpy(gpx->creator, "");
    else
    {
        strcpy(gpx->creator, ss);
    }
    free(ss);

    gpx->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
    gpx->tracks = initializeList(trackToString, deleteTrack, compareTracks);
    gpx->routes = initializeList(routeToString, deleteRoute, compareRoutes);

    parser(doc, root_element->children, &gpx);

    xmlFreeDoc(doc);

    xmlCleanupParser();

    return gpx;
}

void deleteGPXdoc(GPXdoc *doc)
{
    if (doc == NULL)
        return;

    free(doc->creator);
    freeList(doc->waypoints);
    freeList(doc->routes);
    freeList(doc->tracks);
    free(doc);
}

GPXdoc *createValidGPXdoc(char *fileName, char *gpxSchemaFile)
{
    if ((fileName == NULL) || (gpxSchemaFile == NULL) || (strcmp(fileName, "") == 0) || (strcmp(gpxSchemaFile, "") == 0))
    {
        return NULL;
    }

    int ok = 0;

    xmlDocPtr doc;
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    if (schema == NULL)
    {
        ok = 1;
    }

    xmlSchemaFreeParserCtxt(ctxt);

    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL)
    {
        ok = 1;
    }
    else
    {
        xmlSchemaValidCtxtPtr ctxt;
        int return_status;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
        return_status = xmlSchemaValidateDoc(ctxt, doc);

        if (return_status != 0)
        {
            ok = 1;
        }

        xmlSchemaFreeValidCtxt(ctxt);
        xmlFreeDoc(doc);
    }

    if (schema != NULL)
        xmlSchemaFree(schema);

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();

    if (ok == 0)
    {
        GPXdoc *validated_gpxdoc = createGPXdoc(fileName);
        return validated_gpxdoc;
    }
    return NULL;
}

xmlDocPtr get_xmldoc(GPXdoc *gpx_doc)
{
    if (gpx_doc == NULL)
        return NULL;

    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;

    LIBXML_TEST_VERSION;

    doc = xmlNewDoc(BAD_CAST "1.0");

    root_node = xmlNewNode(NULL, BAD_CAST "gpx");
    xmlDocSetRootElement(doc, root_node);

    char *string_version = (char *)malloc(100000);
    sprintf(string_version, "%.1f", gpx_doc->version);
    xmlNewProp(root_node, BAD_CAST "version", BAD_CAST string_version);

    strcpy(string_version, gpx_doc->creator);
    xmlNewProp(root_node, BAD_CAST "creator", BAD_CAST string_version);

    xmlNsPtr ns_ptr;

    ns_ptr = xmlNewNs(root_node, ((const unsigned char *)(gpx_doc->namespace)), NULL);
    xmlSetNs(root_node, ns_ptr);

    if (gpx_doc->waypoints->length != 0)
    {
        ListIterator itr = createIterator(gpx_doc->waypoints);
        Waypoint *el;
        while ((el = nextElement(&itr)) != NULL)
        {
            xmlNodePtr new_node = xmlNewChild(root_node, NULL, BAD_CAST "wpt", NULL);
            snprintf(string_version, 100, "%lf", el->latitude);
            xmlNewProp(new_node, BAD_CAST "lat", BAD_CAST string_version);
            snprintf(string_version, 100, "%lf", el->longitude);
            xmlNewProp(new_node, BAD_CAST "lon", BAD_CAST string_version);

            if (strcmp(el->name, "") != 0)
            {
                xmlNewChild(new_node, NULL, BAD_CAST "name", ((const unsigned char *)(el->name)));
            }
            if (el->otherData->length != 0)
            {
                ListIterator itr1 = createIterator(el->otherData);
                GPXData *sub_el;

                while ((sub_el = nextElement(&itr1)) != NULL)
                {
                    xmlNewChild(new_node, NULL, BAD_CAST sub_el->name, ((const unsigned char *)(sub_el->value)));
                }
            }
        }
    }

    if (gpx_doc->routes->length != 0)
    {
        ListIterator itr_route = createIterator(gpx_doc->routes);
        Route *el_route;
        while ((el_route = nextElement(&itr_route)) != NULL)
        {
            xmlNodePtr new_node = xmlNewChild(root_node, NULL, BAD_CAST "rte", NULL);

            if (strcmp(el_route->name, "") != 0)
            {
                xmlNewChild(new_node, NULL, BAD_CAST "name", ((const unsigned char *)(el_route->name)));
            }
            if (el_route->otherData->length != 0)
            {
                ListIterator itr1 = createIterator(el_route->otherData);
                GPXData *sub_el;
                while ((sub_el = nextElement(&itr1)) != NULL)
                {
                    xmlNewChild(new_node, NULL, BAD_CAST sub_el->name, ((const unsigned char *)(sub_el->value)));
                }
            }

            if (el_route->waypoints->length > 0)
            {
                ListIterator itr = createIterator(el_route->waypoints);
                Waypoint *el;
                while ((el = nextElement(&itr)) != NULL)
                {
                    xmlNodePtr sub_new_node = xmlNewChild(new_node, NULL, BAD_CAST "rtept", NULL);
                    snprintf(string_version, 100, "%lf", el->latitude);
                    xmlNewProp(sub_new_node, BAD_CAST "lat", BAD_CAST string_version);
                    snprintf(string_version, 100, "%lf", el->longitude);
                    xmlNewProp(sub_new_node, BAD_CAST "lon", BAD_CAST string_version);

                    if (strcmp(el->name, "") != 0)
                    {
                        xmlNewChild(sub_new_node, NULL, BAD_CAST "name", ((const unsigned char *)(el->name)));
                    }
                    if (el->otherData->length != 0)
                    {
                        ListIterator itr1 = createIterator(el->otherData);
                        GPXData *sub_el;
                        while ((sub_el = nextElement(&itr1)) != NULL)
                        {
                            xmlNewChild(sub_new_node, NULL, BAD_CAST sub_el->name, ((const unsigned char *)(sub_el->value)));
                        }
                    }
                }
            }
        }
    }

    if (gpx_doc->tracks->length != 0)
    {
        ListIterator itr_track = createIterator(gpx_doc->tracks);
        Track *el_track;

        while ((el_track = nextElement(&itr_track)) != NULL)
        {
            xmlNodePtr new_node = xmlNewChild(root_node, NULL, BAD_CAST "trk", NULL);

            if (strcmp(el_track->name, "") != 0)
            {
                xmlNewChild(new_node, NULL, BAD_CAST "name", ((const unsigned char *)(el_track->name)));
            }
            if (el_track->otherData->length != 0)
            {
                ListIterator itr1 = createIterator(el_track->otherData);
                GPXData *sub_el;
                while ((sub_el = nextElement(&itr1)) != NULL)
                {
                    xmlNewChild(new_node, NULL, BAD_CAST sub_el->name, ((const unsigned char *)(sub_el->value)));
                }
            }

            if (el_track->segments->length > 0)
            {
                ListIterator itr_seg = createIterator(el_track->segments);
                TrackSegment *el_seg;
                while ((el_seg = nextElement(&itr_seg)) != NULL)
                {
                    xmlNodePtr sub_node = xmlNewChild(new_node, NULL, BAD_CAST "trkseg", NULL);

                    if (el_seg->waypoints->length != 0)
                    {
                        ListIterator itr = createIterator(el_seg->waypoints);
                        Waypoint *el;
                        while ((el = nextElement(&itr)) != NULL)
                        {
                            xmlNodePtr new_sub_node = xmlNewChild(sub_node, NULL, BAD_CAST "trkpt", NULL);
                            snprintf(string_version, 100, "%lf", el->latitude);
                            xmlNewProp(new_sub_node, BAD_CAST "lat", BAD_CAST string_version);
                            snprintf(string_version, 100, "%lf", el->longitude);
                            xmlNewProp(new_sub_node, BAD_CAST "lon", BAD_CAST string_version);

                            if (strcmp(el->name, "") != 0)
                            {
                                xmlNewChild(new_sub_node, NULL, BAD_CAST "name", ((const unsigned char *)(el->name)));
                            }
                            if (el->otherData->length != 0)
                            {
                                ListIterator itr1 = createIterator(el->otherData);
                                GPXData *sub_el;
                                while ((sub_el = nextElement(&itr1)) != NULL)
                                {
                                    xmlNewChild(new_sub_node, NULL, BAD_CAST sub_el->name, ((const unsigned char *)(sub_el->value)));
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    free(string_version);
    return doc;
}

bool writeGPXdoc(GPXdoc *gpx_doc, char *fileName)
{
    if ((gpx_doc == NULL) || (fileName == NULL) || (strcmp(fileName, "") == 0))
    {
        return false;
    }

    xmlDocPtr doc = get_xmldoc(gpx_doc);

    xmlSaveFormatFileEnc(fileName, doc, "UTF-8", 1);

    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();

    return true;
}

bool validateGPXDoc(GPXdoc *gpx_doc, char *gpxSchemaFile)
{
    int _false_ = 0;
    if ((gpx_doc == NULL) || (gpxSchemaFile == NULL) || (strcmp(gpxSchemaFile, "") == 0))
    {
        return false;
    }

    if (strcmp(gpx_doc->namespace, "") == 0)
        return false;

    if (strcmp(gpx_doc->creator, "") == 0 || (gpx_doc->creator == NULL))
        return false;

    if ((gpx_doc->waypoints == NULL) || (gpx_doc->routes == NULL) || (gpx_doc->tracks == NULL))
        return false;

    if (gpx_doc->waypoints->length != 0)
    {
        ListIterator itr = createIterator(gpx_doc->waypoints);
        Waypoint *el;
        while ((el = nextElement(&itr)) != NULL)
        {
            if ((el->name == NULL))
            {
                return false;
            }

            if (el->otherData == NULL)
                return false;

            if (el->otherData->length != 0)
            {
                ListIterator itr1 = createIterator(el->otherData);
                GPXData *sub_el;
                while ((sub_el = nextElement(&itr1)) != NULL)
                {
                    if ((strcmp(sub_el->name, "") == 0) || (strcmp(sub_el->value, "") == 0))
                        return false;
                }
            }
        }
    }

    if (gpx_doc->routes->length != 0)
    {
        ListIterator itr_route = createIterator(gpx_doc->routes);
        Route *el_route;
        while ((el_route = nextElement(&itr_route)) != NULL)
        {
            if ((el_route->name == NULL))
            {
                return false;
            }

            if ((el_route->otherData == NULL) || (el_route->waypoints == NULL))
            {
                return false;
            }

            if (el_route->otherData->length != 0)
            {
                ListIterator itr1 = createIterator(el_route->otherData);
                GPXData *sub_el;
                while ((sub_el = nextElement(&itr1)) != NULL)
                {
                    if ((strcmp(sub_el->name, "") == 0) || (strcmp(sub_el->value, "") == 0))
                    {
                        return false;
                    }
                }
            }

            if (el_route->waypoints->length > 0)
            {
                ListIterator itr = createIterator(el_route->waypoints);
                Waypoint *el;
                while ((el = nextElement(&itr)) != NULL)
                {
                    if ((el->name == NULL))
                    {
                        return false;
                    }
                    if (el->otherData == NULL)
                        return false;

                    if (el->otherData->length != 0)
                    {
                        ListIterator itr1 = createIterator(el->otherData);
                        GPXData *sub_el;
                        while ((sub_el = nextElement(&itr1)) != NULL)
                        {
                            if ((strcmp(sub_el->name, "") == 0) || (strcmp(sub_el->value, "") == 0))
                                return false;
                        }
                    }
                }
            }
        }
    }

    if (gpx_doc->tracks->length != 0)
    {
        ListIterator itr_track = createIterator(gpx_doc->tracks);
        Track *el_track;

        while ((el_track = nextElement(&itr_track)) != NULL)
        {
            if ((el_track->name == NULL))
            {
                return false;
            }
            if ((el_track->segments == NULL) || (el_track->otherData == NULL))
                return false;

            if (el_track->otherData->length != 0)
            {
                ListIterator itr1 = createIterator(el_track->otherData);
                GPXData *sub_el;
                while ((sub_el = nextElement(&itr1)) != NULL)
                {
                    if ((strcmp(sub_el->name, "") == 0) || (strcmp(sub_el->value, "") == 0))
                        return false;
                }
            }

            if (el_track->segments->length > 0)
            {
                ListIterator itr_seg = createIterator(el_track->segments);
                TrackSegment *el_seg;
                while ((el_seg = nextElement(&itr_seg)) != NULL)
                {
                    if (el_seg->waypoints == NULL)
                        return false;

                    if (el_seg->waypoints->length != 0)
                    {
                        ListIterator itr = createIterator(el_seg->waypoints);
                        Waypoint *el;
                        while ((el = nextElement(&itr)) != NULL)
                        {
                            if ((el->name == NULL))
                            {
                                return false;
                            }
                            if (el->otherData == NULL)
                                return false;

                            if (el->otherData->length != 0)
                            {
                                ListIterator itr1 = createIterator(el->otherData);
                                GPXData *sub_el;
                                while ((sub_el = nextElement(&itr1)) != NULL)
                                {
                                    if ((strcmp(sub_el->name, "") == 0) || (strcmp(sub_el->value, "") == 0))
                                        return false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (_false_)
        return false;

    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    if (schema == NULL)
    {
        _false_ = 1;
    }

    xmlSchemaFreeParserCtxt(ctxt);

    xmlDocPtr doc = get_xmldoc(gpx_doc);

    if (doc == NULL)
    {
        _false_ = 1;
    }
    else
    {
        xmlSchemaValidCtxtPtr ctxt;
        int return_status;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
        return_status = xmlSchemaValidateDoc(ctxt, doc);

        if (return_status != 0)
        {
            _false_ = 1;
        }

        xmlSchemaFreeValidCtxt(ctxt);
        xmlFreeDoc(doc);
    }

    if (schema != NULL)
        xmlSchemaFree(schema);

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();

    return (_false_ ? false : true);
}

/* <--------------------A2 Module2---------------------> */

float round10(float len)
{
    int exact_len = len;
    int rem = exact_len % 10;

    float result = 0;

    if (rem >= 0 && rem < 5)
    {
        result = exact_len - rem;
    }
    else if (rem >= 5)
    {
        result = exact_len + 10 - rem;
    }
    return result;
}

double haversine_formula(double lat1, double long1, double lat2, double long2)
{
    int radius = 6371 * 1000;
    double phi1 = lat1 * (M_PI / 180);
    double phi2 = lat2 * (M_PI / 180);
    double delta_phi = (lat2 - lat1) * (M_PI / 180);
    double delta_lamda = (long2 - long1) * (M_PI / 180);
    double a = sin(delta_phi / 2) * sin(delta_phi / 2) + cos(phi1) * cos(phi2) * sin(delta_lamda / 2) * sin(delta_lamda / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    double distance = radius * c;
    return distance;
}

float getRouteLen(const Route *rt)
{
    if (rt == NULL)
        return 0;

    if (rt->waypoints == NULL || rt->waypoints->length < 2)
        return 0;

    float result = 0;
    int i = 0;
    ListIterator itr = createIterator(rt->waypoints);
    Waypoint *el1;

    double initial_lat = 0, initial_long = 0;
    while ((el1 = nextElement(&itr)) != NULL)
    {
        if (i == 0)
        {
            initial_lat = el1->latitude;
            initial_long = el1->longitude;
            i++;
            continue;
        }

        result += haversine_formula(initial_lat, initial_long, el1->latitude, el1->longitude);
        initial_lat = el1->latitude;
        initial_long = el1->longitude;
    }

    return result;
}

float getTrackLen(const Track *tr)
{
    if (tr == NULL)
        return 0;

    if (tr->segments == NULL || tr->segments->length == 0)
    {
        return 0;
    }

    float result = 0;

    ListIterator itr_seg = createIterator(tr->segments);
    TrackSegment *el_seg;

    double last_lat = -10000, last_long = -10000;

    while ((el_seg = nextElement(&itr_seg)) != NULL)
    {
        if (el_seg->waypoints->length == 0)
            continue;

        if (el_seg->waypoints->length == 1)
        {
            ListIterator itr = createIterator(el_seg->waypoints);
            Waypoint *el;
            while ((el = nextElement(&itr)) != NULL)
            {
                if (last_lat != -10000 && last_long != -10000)
                {
                    result += haversine_formula(last_lat, last_long, el->latitude, el->longitude);
                }
                last_long = el->longitude;
                last_lat = el->latitude;
            }
            continue;
        }

        double initial_lat = 0, initial_long = 0;
        int i = 1;
        if (el_seg->waypoints->length > 1)
        {
            ListIterator itr = createIterator(el_seg->waypoints);
            Waypoint *el;
            while ((el = nextElement(&itr)) != NULL)
            {
                if (i == 1)
                {
                    if (last_lat != -10000 && last_long != -10000)
                    {
                        result += haversine_formula(last_lat, last_long, el->latitude, el->longitude);
                    }
                    i++;
                    initial_lat = el->latitude;
                    initial_long = el->longitude;
                    continue;
                }
                if (i == el_seg->waypoints->length)
                {
                    last_lat = el->latitude;
                    last_long = el->longitude;
                }
                i++;
                result += haversine_formula(initial_lat, initial_long, el->latitude, el->longitude);
                initial_lat = el->latitude;
                initial_long = el->longitude;
            }
        }
    }

    return result;
}

int numRoutesWithLength(const GPXdoc *doc, float len, float delta)
{
    int count_routes = 0;

    if (doc == NULL || len < 0 || delta < 0)
        return 0;

    if (doc->routes->length == 0)
        return 0;

    if (doc->routes->length > 0)
    {
        ListIterator itr_route = createIterator(doc->routes);
        Route *el_route;
        while ((el_route = nextElement(&itr_route)) != NULL)
        {
            float this_length = getRouteLen(el_route);
            if (fabs(this_length - len) <= delta)
                count_routes += 1;
        }
    }

    return count_routes;
}

int numTracksWithLength(const GPXdoc *doc, float len, float delta)
{
    int count_tracks = 0;

    if (doc == NULL || len < 0 || delta < 0)
        return 0;

    if (doc->tracks->length == 0)
        return 0;

    if (doc->tracks->length > 0)
    {
        ListIterator itr_track = createIterator(doc->tracks);
        Track *el_track;
        while ((el_track = nextElement(&itr_track)) != NULL)
        {
            float this_length = getTrackLen(el_track);
            printf("%f\n", fabs(this_length - len));
            if (fabs(this_length - len) <= delta)
                count_tracks += 1;
        }
    }
    return count_tracks;
}

bool isLoopRoute(const Route *route, float delta)
{
    if (route == NULL || delta < 0)
        return false;

    if (route->waypoints->length < 4)
        return false;

    double in_lat, in_lon, last_lat, last_lon;

    ListIterator itr = createIterator(route->waypoints);
    Waypoint *el;
    int i = 1;
    while ((el = nextElement(&itr)) != NULL)
    {
        if (i == 1)
        {
            in_lat = el->latitude;
            in_lon = el->longitude;
        }
        if (i == route->waypoints->length)
        {
            last_lat = el->latitude;
            last_lon = el->longitude;
        }
        i++;
    }

    float result = 0;
    result += haversine_formula(in_lat, in_lon, last_lat, last_lon);

    return (result <= delta ? true : false);
}

bool isLoopTrack(const Track *tr, float delta)
{
    if (tr == NULL || delta < 0)
        return false;

    if (tr->segments->length == 0)
        return false;

    double in_lat, in_lon, last_lat, last_lon;

    ListIterator itr_seg = createIterator(tr->segments);
    TrackSegment *el_seg;

    int number_of_waypoints = 0;

    while ((el_seg = nextElement(&itr_seg)) != NULL)
    {
        number_of_waypoints += el_seg->waypoints->length;
    }

    if (number_of_waypoints < 4)
        return false;

    ListIterator itr_seg_new = createIterator(tr->segments);
    TrackSegment *el_seg_new;

    int i = 1;
    while ((el_seg_new = nextElement(&itr_seg_new)) != NULL)
    {
        if (el_seg_new->waypoints->length == 0)
            continue;

        ListIterator itr = createIterator(el_seg_new->waypoints);
        Waypoint *el;
        while ((el = nextElement(&itr)) != NULL)
        {
            if (i == 1)
            {
                in_lat = el->latitude;
                in_lon = el->longitude;
            }
            if (i == number_of_waypoints)
            {
                last_lat = el->latitude;
                last_lon = el->longitude;
            }
            i++;
        }
    }

    float result = 0;
    result += haversine_formula(in_lat, in_lon, last_lat, last_lon);

    return (result <= delta ? true : false);
}

void delete_dummy()
{
    return;
}

List *getRoutesBetween(const GPXdoc *doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta)
{
    int added = 0;

    if (doc == NULL || delta < 0)
        return NULL;

    if (doc->routes == NULL || doc->routes->length == 0)
    {
        return NULL;
    }
    List *between_routes = initializeList(routeToString, delete_dummy, compareRoutes);

    ListIterator itr_route = createIterator(doc->routes);
    Route *el_route;

    while ((el_route = nextElement(&itr_route)) != NULL)
    {
        int source_found = 0;
        int dest_found = 0;

        if (el_route->waypoints->length == 0)
            continue;

        ListIterator itr = createIterator(el_route->waypoints);
        Waypoint *el;

        while ((el = nextElement(&itr)) != NULL)
        {
            float source_dist = 0, dest_dist = 0;

            source_dist += haversine_formula(sourceLat, sourceLong, el->latitude, el->longitude);
            dest_dist += haversine_formula(destLat, destLong, el->latitude, el->longitude);

            if (source_dist <= delta)
                source_found = 1;

            if (dest_dist <= delta)
                dest_found = 1;
        }

        if ((source_found == 1) && (dest_found == 1))
        {
            added += 1;
            insertBack(between_routes, el_route);
        }
    }

    if (added == 0)
    {
        freeList(between_routes);
        return NULL;
    }
    return between_routes;
}

List *getTracksBetween(const GPXdoc *doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta)
{
    if (doc == NULL || delta < 0)
        return NULL;

    if (doc->tracks == NULL || doc->tracks->length == 0)
    {
        return NULL;
    }

    int added = 0;

    List *between_tracks = initializeList(trackToString, delete_dummy, compareTracks);

    ListIterator itr_trk = createIterator(doc->tracks);
    Track *el_trk;

    while ((el_trk = nextElement(&itr_trk)) != NULL)
    {
        bool source_found = false;
        bool dest_found = false;

        ListIterator itr_seg = createIterator(el_trk->segments);
        TrackSegment *el_seg;
        while ((el_seg = nextElement(&itr_seg)) != NULL)
        {
            if (el_seg->waypoints->length != 0)
            {
                ListIterator itr = createIterator(el_seg->waypoints);
                Waypoint *el;
                while ((el = nextElement(&itr)) != NULL)
                {
                    float source_dist = 0, dest_dist = 0;

                    source_dist += haversine_formula(sourceLat, sourceLong, el->latitude, el->longitude);
                    dest_dist += haversine_formula(destLat, destLong, el->latitude, el->longitude);

                    if (source_dist <= delta)
                        source_found = true;

                    if (dest_dist <= delta)
                        dest_found = true;
                }
            }
        }

        if (source_found && dest_found)
        {
            added++;
            insertBack(between_tracks, el_trk);
        }
    }

    if (added == 0)
    {
        freeList(between_tracks);
        return NULL;
    }
    return between_tracks;
}

char *routeToJSON(const Route *rt)
{
    char *rjson = (char *)malloc(100000);
    strcpy(rjson, "");

    strcat(rjson, "{");

    if (rt != NULL)
    {
        char quotes = 34;
        char *temp = (char *)malloc(100000);
        sprintf(temp, "%cname%c:", quotes, quotes);
        strcat(rjson, temp);
        if ((rt->name == NULL) || strcmp(rt->name, "") == 0)
        {
            sprintf(temp, "%cNone%c", quotes, quotes);
            strcat(rjson, temp);
        }
        else
        {
            sprintf(temp, "%c%s%c", quotes, rt->name, quotes);
            strcat(rjson, temp);
        }
        sprintf(temp, ",%cnumPoints%c:", quotes, quotes);
        strcat(rjson, temp);

        int num_pts = rt->waypoints->length;

        sprintf(temp, "%d", num_pts);
        strcat(rjson, temp);

        sprintf(temp, ",%clen%c:", quotes, quotes);
        strcat(rjson, temp);

        float length = getRouteLen(rt);
        float rounded_length = round10(length);

        sprintf(temp, "%.1f", rounded_length);
        strcat(rjson, temp);

        sprintf(temp, ",%cloop%c:", quotes, quotes);
        strcat(rjson, temp);

        bool loop = false;

        loop = isLoopRoute(rt, 10);

        (loop ? strcpy(temp, "true") : strcpy(temp, "false"));
        strcat(rjson, temp);

        free(temp);
    }

    strcat(rjson, "}");
    return rjson;
}

char *trackToJSON(const Track *tr)
{
    char *tjson = (char *)malloc(100000);
    strcpy(tjson, "");

    strcat(tjson, "{");

    if (tr != NULL)
    {
        char quotes = 34;
        char *temp = (char *)malloc(100000);
        sprintf(temp, "%cname%c:", quotes, quotes);
        strcat(tjson, temp);
        if ((tr->name == NULL) || strcmp(tr->name, "") == 0)
        {
            sprintf(temp, "%cNone%c", quotes, quotes);
            strcat(tjson, temp);
        }
        else
        {
            sprintf(temp, "%c%s%c", quotes, tr->name, quotes);
            strcat(tjson, temp);
        }

        ListIterator itr_seg = createIterator(tr->segments);
        TrackSegment *el_seg;

        int numPoints = 0;

        while ((el_seg = nextElement(&itr_seg)) != NULL)
        {
            if (el_seg->waypoints->length != 0)
            {
                numPoints += el_seg->waypoints->length;
            }
        }

        sprintf(temp, ",%cnumPoints%c:%d", quotes, quotes, numPoints);
        strcat(tjson, temp);

        sprintf(temp, ",%clen%c:", quotes, quotes);
        strcat(tjson, temp);

        float length = getTrackLen(tr);
        float rounded_length = round10(length);

        sprintf(temp, "%.1f", rounded_length);
        strcat(tjson, temp);

        sprintf(temp, ",%cloop%c:", quotes, quotes);
        strcat(tjson, temp);

        bool loop = false;

        loop = isLoopTrack(tr, 10);

        (loop ? strcpy(temp, "true") : strcpy(temp, "false"));
        strcat(tjson, temp);

        free(temp);
    }

    strcat(tjson, "}");
    return tjson;
}

char *routeListToJSON(const List *list)
{
    List *routelist = (List *)(list);
    char *rjson = (char *)malloc(1000000);
    strcpy(rjson, "");

    strcat(rjson, "[");

    if (routelist != NULL)
    {
        ListIterator itr_rt = createIterator(routelist);
        Route *el_rt;
        int l = routelist->length;
        int i = 1;
        while ((el_rt = nextElement(&itr_rt)) != NULL)
        {
            char *for_this_route = routeToJSON(el_rt);
            strcat(rjson, for_this_route);
            free(for_this_route);
            if (i != l)
                strcat(rjson, ",");
            i++;
        }
    }

    strcat(rjson, "]");
    return rjson;
}

char *trackListToJSON(const List *list)
{
    List *tracklist = (List *)(list);
    char *tjson = (char *)malloc(1000000);
    strcpy(tjson, "");

    strcat(tjson, "[");

    if (tracklist != NULL)
    {
        ListIterator itr_tr = createIterator(tracklist);
        Track *el_tr;
        int l = tracklist->length;
        int i = 1;
        while ((el_tr = nextElement(&itr_tr)) != NULL)
        {
            char *for_this_track = trackToJSON(el_tr);
            strcat(tjson, for_this_track);
            free(for_this_track);
            if (i != l)
                strcat(tjson, ",");
            i++;
        }
    }

    strcat(tjson, "]");
    return tjson;
}

char *GPXtoJSON(const GPXdoc *gpx)
{
    char *gjson = (char *)malloc(100000);
    strcpy(gjson, "");

    strcat(gjson, "{");

    if (gpx != NULL)
    {
        char quotes = 34;
        char *temp = (char *)malloc(100000);
        sprintf(temp, "%cversion%c:", quotes, quotes);
        strcat(gjson, temp);

        sprintf(temp, "%.1f", gpx->version);
        strcat(gjson, temp);

        sprintf(temp, ",%ccreator%c:", quotes, quotes);
        strcat(gjson, temp);

        sprintf(temp, "%c%s%c", quotes, gpx->creator, quotes);
        strcat(gjson, temp);

        sprintf(temp, ",%cnumWaypoints%c:", quotes, quotes);
        strcat(gjson, temp);

        int num_pts = gpx->waypoints->length;

        sprintf(temp, "%d", num_pts);
        strcat(gjson, temp);

        sprintf(temp, ",%cnumRoutes%c:", quotes, quotes);
        strcat(gjson, temp);

        num_pts = gpx->routes->length;

        sprintf(temp, "%d", num_pts);
        strcat(gjson, temp);

        sprintf(temp, ",%cnumTracks%c:", quotes, quotes);
        strcat(gjson, temp);

        num_pts = gpx->tracks->length;

        sprintf(temp, "%d", num_pts);
        strcat(gjson, temp);

        free(temp);
    }

    strcat(gjson, "}");
    return gjson;
}

void addWaypoint(Route *rt, Waypoint *pt)
{
    if (rt == NULL || pt == NULL)
        return;

    insertBack(rt->waypoints, pt);
}

void addRoute(GPXdoc *doc, Route *rt)
{
    if (doc == NULL || rt == NULL)
        return;

    insertBack(doc->routes, rt);
}

GPXdoc *JSONtoGPX(const char *gpxString)
{
    if ((gpxString == NULL) || (strcmp(gpxString, "{}") == 0) || (strcmp(gpxString, "") == 0))
        return NULL;

    GPXdoc *gpx = (GPXdoc *)malloc(sizeof(GPXdoc));

    int semicolon_number = 0;

    for (int i = 0; i < strlen(gpxString); i++)
    {
        if (gpxString[i] == ':' && semicolon_number == 0)
        {
            semicolon_number += 1;
            char *s = (char *)malloc(100000);
            strcpy(s, "");
            i += 1;
            while (gpxString[i] != ',')
            {
                strncat(s, &gpxString[i], 1);
                i++;
            }
            char *ptr;
            double l;
            l = strtod(s, &ptr);
            gpx->version = l;
            free(s);
        }

        if (gpxString[i] == ':' && semicolon_number == 1)
        {
            semicolon_number += 1;
            char *s = (char *)malloc(100000);
            strcpy(s, "");
            i += 2;
            while (gpxString[i] != 34)
            {
                strncat(s, &gpxString[i], 1);
                i++;
            }
            gpx->creator = (char *)malloc(100000);

            strcpy(gpx->creator, s);
            free(s);
        }
    }

    strcpy(gpx->namespace, "http://www.topografix.com/GPX/1/1");

    gpx->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
    gpx->tracks = initializeList(trackToString, deleteTrack, compareTracks);
    gpx->routes = initializeList(routeToString, deleteRoute, compareRoutes);

    return gpx;
}

Waypoint *JSONtoWaypoint(const char *gpxString)
{
    if ((gpxString == NULL) || (strcmp(gpxString, "{}") == 0) || (strcmp(gpxString, "") == 0))
        return NULL;

    Waypoint *wp = (Waypoint *)malloc(sizeof(Waypoint));
    wp->name = (char *)malloc(sizeof(char) * 10000);
    strcpy(wp->name, "");
    wp->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData);

    int semicolon_number = 0;

    for (int i = 0; i < strlen(gpxString); i++)
    {
        if (gpxString[i] == ':' && semicolon_number == 0)
        {
            semicolon_number += 1;
            char *s = (char *)malloc(100000);
            strcpy(s, "");
            i += 1;
            while (gpxString[i] != ',')
            {
                strncat(s, &gpxString[i], 1);
                i++;
            }
            char *ptr;
            double lat;
            lat = strtod(s, &ptr);
            wp->latitude = lat;
            free(s);
        }

        if (gpxString[i] == ':' && semicolon_number == 1)
        {
            semicolon_number += 1;
            char *s = (char *)malloc(100000);
            strcpy(s, "");
            i += 1;
            while (gpxString[i] != '}')
            {
                strncat(s, &gpxString[i], 1);
                i++;
            }
            char *ptr;
            double lon;
            lon = strtod(s, &ptr);
            wp->longitude = lon;
            free(s);
        }
    }

    return wp;
}

Route *JSONtoRoute(const char *gpxString)
{
    if ((gpxString == NULL) || (strcmp(gpxString, "{}") == 0) || (strcmp(gpxString, "") == 0))
        return NULL;

    rt = (Route *)malloc(sizeof(Route));
    rt->name = (char *)malloc(100000);
    strcpy(rt->name, "");
    rt->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
    rt->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData);

    int semicolon_number = 0;

    for (int i = 0; i < strlen(gpxString); i++)
    {
        if (gpxString[i] == ':' && semicolon_number == 0)
        {
            semicolon_number += 1;
            char *s = (char *)malloc(100000);
            strcpy(s, "");
            i += 2;
            while (gpxString[i] != 34)
            {
                strncat(s, &gpxString[i], 1);
                i++;
            }
            strcpy(rt->name, s);
            free(s);
        }
    }

    return rt;
}

/* New Functions to be used in Node.JS Backend */

bool validate_gpx_file(char *filename, char *gpxSchemaFile)
{
    GPXdoc *temp_doc = createValidGPXdoc(filename, gpxSchemaFile);

    if (temp_doc == NULL)
    {
        return false;
    }
    deleteGPXdoc(temp_doc);
    return true;
}

/* Already Validated */
char *gpxfile_to_json(char *filename)
{
    GPXdoc *temp = createGPXdoc(filename);
    char *s = (char *)malloc(1000);
    strcpy(s, "");

    s = GPXtoJSON(temp);
    deleteGPXdoc(temp);
    return s;
}

char *get_all_routes(char *filename)
{
    GPXdoc *doc = createGPXdoc(filename);
    char *s = (char *)malloc(100000);
    char *temp = (char *)malloc(10000);
    strcpy(s, "");

    s = routeListToJSON(doc->routes);
    deleteGPXdoc(doc);
    free(temp);
    return s;
}

char *get_all_tracks(char *filename)
{
    GPXdoc *doc = createGPXdoc(filename);
    char *s = (char *)malloc(100000);
    char *temp = (char *)malloc(10000);
    strcpy(s, "");

    s = trackListToJSON(doc->tracks);
    deleteGPXdoc(doc);
    free(temp);
    return s;
}

char *dropdown_list(char *filename)
{
    GPXdoc *doc = createGPXdoc(filename);
    char *s = (char *)malloc(10000);
    strcpy(s, "");
    strcat(s, "{");
    for (int i = 1; i <= doc->routes->length; i++)
    {
        char *temp = (char *)malloc(10000);
        char c = '"';
        if (i == doc->routes->length && doc->tracks->length == 0)
        {
            sprintf(temp, "%cRoute%d%c:%cRoute%d%c", c, i, c, c, i, c);
            strcat(s, temp);
            free(temp);
            continue;
        }
        sprintf(temp, "%cRoute%d%c:%cRoute%d%c,", c, i, c, c, i, c);
        strcat(s, temp);
        free(temp);
    }
    for (int i = 1; i <= doc->tracks->length; i++)
    {
        char *temp = (char *)malloc(10000);
        char c = '"';
        if (i == doc->tracks->length)
        {
            sprintf(temp, "%cTrack%d%c:%cTrack%d%c", c, i, c, c, i, c);
        }
        else
        {
            sprintf(temp, "%cTrack%d%c:%cTrack%d%c,", c, i, c, c, i, c);
        }
        strcat(s, temp);
        free(temp);
    }
    strcat(s, "}");
    deleteGPXdoc(doc);
    return s;
}

bool name_change_gpx(char *filename, char *name_change, char *component, int id)
{
    int done = 0;
    GPXdoc *doc = createGPXdoc(filename);
    if (component[0] == 'T')
    {
        ListIterator itr_tr = createIterator(doc->tracks);
        Track *el_tr;
        int i = 1;
        while ((el_tr = nextElement(&itr_tr)) != NULL)
        {
            if (id == i)
            {
                strcpy(el_tr->name, name_change);
                done = 1;
                writeGPXdoc(doc, filename);
                break;
            }
            i++;
        }
    }

    if (component[0] == 'R')
    {
        ListIterator itr_tr = createIterator(doc->routes);
        Route *el_tr;
        int i = 1;
        while ((el_tr = nextElement(&itr_tr)) != NULL)
        {
            if (id == i)
            {
                strcpy(el_tr->name, name_change);
                done = 1;
                writeGPXdoc(doc, filename);
                break;
            }
            i++;
        }
    }

    deleteGPXdoc(doc);
    return (done ? true : false);
}

char *other_data(char *filename, char *component, int id)
{
    GPXdoc *doc = createGPXdoc(filename);
    char *s = (char *)malloc(10000);
    strcpy(s, "");
    strcat(s, "[");
    if (component[0] == 'T')
    {
        ListIterator itr_tr = createIterator(doc->tracks);
        Track *el_tr;
        int i = 1;
        while ((el_tr = nextElement(&itr_tr)) != NULL)
        {
            if (id == i)
            {
                ListIterator itr = createIterator(el_tr->otherData);
                GPXData *el;
                int l = 1;
                while ((el = nextElement(&itr)) != NULL)
                {
                    char *temp = (char *)malloc(10000);
                    char c = '"';

                    if (l == el_tr->otherData->length)
                    {
                        sprintf(temp, "{%cname%c:%c%s%c,%cvalue%c:%c%s%c}", c, c, c, el->name, c, c, c, c, el->value, c);
                        strcat(s, temp);
                    }
                    else
                    {
                        sprintf(temp, "{%cname%c:%c%s%c,%cvalue%c:%c%s%c},", c, c, c, el->name, c, c, c, c, el->value, c);
                        strcat(s, temp);
                    }
                    free(temp);
                }

                break;
            }
            i++;
        }
    }

    if (component[0] == 'R')
    {
        ListIterator itr_tr = createIterator(doc->routes);
        Route *el_tr;
        int i = 1;
        while ((el_tr = nextElement(&itr_tr)) != NULL)
        {
            if (id == i)
            {
                ListIterator itr = createIterator(el_tr->otherData);
                GPXData *el;
                int l = 1;
                while ((el = nextElement(&itr)) != NULL)
                {

                    char *temp = (char *)malloc(10000);
                    char c = '"';
                    if (l == el_tr->otherData->length)
                    {
                        sprintf(temp, "{%cname%c:%c%s%c,%cvalue%c:%c%s%c}", c, c, c, el->name, c, c, c, c, el->value, c);
                        strcat(s, temp);
                    }
                    else
                    {
                        sprintf(temp, "{%cname%c:%c%s%c,%cvalue%c:%c%s%c},", c, c, c, el->name, c, c, c, c, el->value, c);
                        strcat(s, temp);
                    }
                    free(temp);
                }

                break;
            }
            i++;
        }
    }

    strcat(s, "]");
    deleteGPXdoc(doc);
    return s;
}

char *find_path(char *filename, char *name, double lat1, double long1, double lat2, double long2, double tol)
{
    GPXdoc *doc = createGPXdoc(filename);

    char *s = (char *)malloc(10000);
    strcpy(s, "");
    strcat(s, "[");

    List *gr = getRoutesBetween(doc, lat1, long1, lat2, long2, tol);

    int j = 1;
    if (gr != NULL)
    {
        ListIterator itr_tr = createIterator(gr);
        Route *el_tr;
        while ((el_tr = nextElement(&itr_tr)) != NULL)
        {
            int i = 1, num = 0;

            ListIterator itr = createIterator(doc->routes);
            Route *el;
            while ((el = nextElement(&itr)) != NULL)
            {
                if (el_tr == el)
                {
                    num = i;
                    break;
                }
                i++;
            }
            char *temp = (char *)malloc(10000);
            char c = '"';
            sprintf(temp, "[{%cname%c:%c%s%c,%cnum%c:%cRoute%d%c},", c, c, c, name, c, c, c, c, num, c);
            strcat(s, temp);

            temp = routeToJSON(el_tr);
            strcat(s, temp);
            if (j == gr->length)
            {
                strcat(s, "]");
            }
            else
            {
                strcat(s, "],");
            }
            j++;
        }
    }

    freeList(gr);

    List *gt = getTracksBetween(doc, lat1, long1, lat2, long2, tol);

    j = 1;
    if (gt != NULL)
    {
        ListIterator itr_tr = createIterator(gt);
        Track *el_tr;
        while ((el_tr = nextElement(&itr_tr)) != NULL)
        {
            int i = 1, num = 0;

            ListIterator itr = createIterator(doc->tracks);
            Track *el;
            while ((el = nextElement(&itr)) != NULL)
            {
                if (el_tr == el)
                {
                    num = i;
                    break;
                }
                i++;
            }
            char *temp = (char *)malloc(10000);
            char c = '"';
            sprintf(temp, ",[{%cname%c:%c%s%c,%cnum%c:%cTrack%d%c},", c, c, c, name, c, c, c, c, num, c);
            strcat(s, temp);

            temp = trackToJSON(el_tr);
            strcat(s, temp);
            strcat(s, "]");

            j++;
        }
    }

    freeList(gt);

    strcat(s, "]");
    deleteGPXdoc(doc);
    return s;
}

bool CreateGPXFile(char *filename, char *gpxSchema, char *jsonStr)
{
    int ok = 0;
    GPXdoc *doc = JSONtoGPX(jsonStr);
    bool valid = validateGPXDoc(doc, gpxSchema);

    if (valid != false)
    {
        ok = 1;
    }

    bool written = writeGPXdoc(doc, filename);

    if (written != false)
    {
        ok = 1;
    }
    deleteGPXdoc(doc);
    return ok;
}

bool ADDrouteToFile(char *filename, char *rtname, char *wplist, char *GPXSchema)
{
    GPXdoc *doc = createGPXdoc(filename);

    Route *rt = JSONtoRoute(rtname);
    int i = 0;
    while (i < strlen(wplist))
    {
        if (wplist[i] == '{')
        {
            char *temp = (char *)malloc(1000);
            int j, k = 0;
            for (j = i; wplist[j] != '}'; j++)
            {
                temp[k++] = wplist[j];
            }
            temp[j] = '}';

            i = j;

            Waypoint *wp = JSONtoWaypoint(temp);
            addWaypoint(rt, wp);
            free(temp);
        }
        else
        {
            i++;
        }
    }

    addRoute(doc, rt);

    int ok = 0;
    bool valid = validateGPXDoc(doc, GPXSchema);

    if (valid)
    {
        bool written = writeGPXdoc(doc, filename);
        if (written)
        {
            ok = 1;
        }
    }

    deleteGPXdoc(doc);
    return ok;
}

/* ------------------------- A4 Functions ------------------ */

char *listWP_Route(char *filename, int route_idx)
{
    GPXdoc *doc = createGPXdoc(filename);

    char *s = (char *)malloc(100000);
    strcpy(s, "");
    strcat(s, "[");

    int i = 1;
    ListIterator itr_tr = createIterator(doc->routes);
    Route *el_tr;
    while ((el_tr = nextElement(&itr_tr)) != NULL)
    {
        if (i == route_idx)
        {
            int j = 0;
            ListIterator itr = createIterator(el_tr->waypoints);
            Waypoint *el;
            char* temp = malloc(10000);
            char c = '"';
            while ((el = nextElement(&itr)) != NULL)
            {   
                if(j < el_tr->waypoints->length - 1)
                    sprintf(temp,"{%cname%c:%c%s%c,%clat%c:%lf,%clon%c:%lf,%cindex%c:%d},",c,c,c,el->name,c,c,c,el->latitude,c,c,el->longitude,c,c,j);
                else
                {
                    sprintf(temp,"{%cname%c:%c%s%c,%clat%c:%lf,%clon%c:%lf,%cindex%c:%d}",c,c,c,el->name,c,c,c,el->latitude,c,c,el->longitude,c,c,j);
                }
                j++;
                strcat(s,temp);
            }
            free(temp);
            break;
        }
        i++;
    }

    strcat(s, "]");
    deleteGPXdoc(doc);

    return s;
}